#include "stdafx.h"
#include "Grunt.h"
#include "Player.h"
#include "AStarManager.h"
#include "Grunt_Slash.h"
#include "CollisionComponent.h"
#include "PhysicTransformComponent.h"
#include "RenderComponent.h"
#include "ComponentManager.h"
#include "ObjectManager.h"
#include "Time.h"
#include "GraphicDevice.h"

using namespace std;

OBJECT_ID::EID Grunt::GetID()
{
	return OBJECT_ID::GRUNT;
}
OBJECT_TAG::ETAG Grunt::GetTag()
{
	return Super::GetTag();
}
std::wstring_view Grunt::GetName() const&
{
	return L"Grunt"sv;
}

void Grunt::Initialize() & noexcept
{
	Super::Initialize();

	_TransformComp->Scale *= 2.5f;
	_PhysicComp->Mass = 100.f;
	

	_RenderComp->bRender = true;
	
	_RenderComp->AfterImgOff();
	_RenderComp->PositionCorrection = vec3{ 0.f,-10.f,0.f };
	
	_CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 35;
	_CollisionComp->_CollisionInfo.Width = 18;

	_PhysicComp->bGravity = true;

	Speed = 500.f ;
	MoveGoalTime = 2.f;
	DetectionRange = 400.f;
	AttackRange = 100.f;

	_CurrentState = Grunt::State::Idle;

	PursuitRange = 700.f;
	NarrowRange = 180.f;

	_SpAttack = ObjectManager::instance().InsertObject < Grunt_Slash>();
	_SpAttack->SetOwner(_This);
	DelayAfterAttack = 0.4f;
	//_RenderComp->SlowColor = D3DCOLOR_ARGB(255, 255, 0, 0);

}

void Grunt::Update()
{
	if (!global::IsPlay())return;
	if ( ! ObjectManager::instance().bEnemyUpdate)return;

	Super::Update();

	FSM();
}

void Grunt::LateUpdate()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;


	Super::LateUpdate();

	const float Dt = Time::instance().Delta();

	//bFrameCurrentCharacterInput = false;
	//bMoveKeyCheck = false;
	//bJumpKeyCheck = false;
	//bAttackKeyCheck = false;
	//bDownKeyCheck = false;
	//bSneakKeyCheck = false;
	//bCurWallRideCollision = false;

	//CurAttackCoolTime -= Dt;
}

void Grunt::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CurrentState == Grunt::State::Walk && _CollisionInfo._ID ==OBJECT_ID::ETILE)
	{
		if (
			( math::almost_equal(vec3{ 1.f,0.f,0.f }, _CollisionInfo.Normal) 
			  && (D3DXVec3Dot(&_CollisionInfo.PosDir, &vec3{ 1.f,0.f,0.f }) > cosf(math::PI / 5.f)))         ||
			(math::almost_equal(vec3{ -1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &vec3{ -1.f,0.f,0.f }) > cosf(math::PI / 5.f)))
			)
		{
			Turn();
			_PhysicComp->Dir.x *= -1.f;
		}
	}
}

void Grunt::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target  , _CollisionInfo );
}

void Grunt::Move(vec3 Dir, const float AddSpeed)
{ 
	Super::Move(Dir  , AddSpeed);
}

void Grunt::Die()&
{
	HurtFly();

	bBlooding = true;
}

void Grunt::EnterStair()
{
	_CurrentState = Grunt::State::EnterStair;
	RenderComponent::NotifyType _Notify;
	_Notify[8] = [this]()
	{
		bEnterStairMotionEnd = true;
	}; 
	_RenderComp->Anim(false, false, L"spr_grunt_enterstair", 8, 0.5f, std::move(_Notify));
}

void Grunt::LeaveStair()
{
	_CurrentState = Grunt::State::LeaveStair;
	RenderComponent::NotifyType _Notify;
	_Notify[8] = [this]()
	{
		bLeaveStairMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_grunt_leavestair", 8, 0.5f, std::move(_Notify));
}

void Grunt::EnterStairState()
{
	if (bEnterStairMotionEnd)
	{
		bEnterStairMotionEnd = false;
		_PhysicComp->Position = LeaveDoorLocation;
		LeaveStair();
	}
}

void Grunt::LeaveStairState()
{
	if (bLeaveStairMotionEnd)
	{
		bLeaveStairMotionEnd = false;
		Run();
	}
}

void Grunt::Attack()
{
	constexpr float AttackRich = 20.f;

	_CurrentState = Grunt::State::Attack;
	RenderComponent::NotifyType _Notify;
	vec3 ToTarget =_Target->_TransformComp->Position;
	ToTarget -= _PhysicComp->Position;
	ToTarget = ConvertXAxisDir(ToTarget);

	_Notify[4] = [ToTarget,this]()
	{
		bAttackMotionEnd = true;
		_SpAttack->AttackStart(ToTarget * AttackRich, ToTarget);
	};
	_RenderComp->Anim(false, false, L"spr_grunt_attack", 8, 0.6f, std::move(_Notify));
	AtTheAttackDir = _PhysicComp->Dir;
};

void Grunt::AttackState()
{
	if (bAttackMotionEnd)
	{
		bAttackMotionEnd = false;

		vec3 ToTarget = _Target->_PhysicComp->Position - _PhysicComp->Position;

		if (D3DXVec3Dot(&AtTheAttackDir, &ToTarget) < 0.f)
		{
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			Turn();
			return;
		}

		Time::instance().TimerRegist(DelayAfterAttack, DelayAfterAttack, DelayAfterAttack, 
		[this]() {
			if (!global::IsPlay())return true;
			if (_EnemyState != NormalEnemy::State::Die)
			{
				Run();
			}
			return true; 
		});
	}
}

void Grunt::Fall()
{

}

void Grunt::FallState()
{

}

void Grunt::HurtFly()
{
	_CurrentState = Grunt::State::HurtFly;
	_RenderComp->Anim(false, false, L"spr_grunt_hurtfly", 2, 0.5f);
	_PhysicComp->Flying();
}

void Grunt::HurtFlyState()
{
	if (_PhysicComp->bLand)
	{
		bBlooding = false;
		HurtGround();
	}
}

void Grunt::HurtGround()
{
	_CurrentState = Grunt::State::HurtGround;
	RenderComponent::NotifyType _Notify;
	bBloodingOverHead = true;

	_Notify[16] = [this]()
	{
		_RenderComp->bSlowRender = true;
		bHurtGroundMotionEnd = true;
	};

	_Notify[9] = [this](){bBloodingOverHead = false;};

	_RenderComp->Anim(false, false, L"spr_grunt_hurtground",
		16, 1.6f, std::move(_Notify));
}

void Grunt::HurtGroundState()
{
	if (bHurtGroundMotionEnd)
	{
		bHurtGroundMotionEnd = false;
	}
}

void Grunt::Idle()
{
	_CurrentState = Grunt::State::Idle;
	_RenderComp->Anim(false, true, L"spr_grunt_idle", 8, 0.5f);
}

void Grunt::IdleState()
{
	vec3 BeforeDir = _PhysicComp->Dir;
	IsDetected = IsRangeInnerTarget();
	

	if (IsDetected)
	{
		if (D3DXVec3Dot(&BeforeDir, &ToTarget) < 0.f)
		{
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			Turn();
			return;
		}
		else 
			Run();
	}
}

void Grunt::Run()
{
	if (!global::IsPlay())return;

	_CurrentState = Grunt::State::Run;
	_RenderComp->Anim(false, true, L"spr_grunt_run", 10, 0.6f);
	bLaziness = false;
};

void Grunt::RunState()
{
	vec3 ToTarget = _Target->_TransformComp->Position - _PhysicComp->Position;
	float ToTargetDistance = D3DXVec3Length(&ToTarget);
	if (ToTargetDistance < AttackRange)
	{
		Attack();
		return;
	};

	vec3 GoalTargetDiff = GoalPos - _Target->_TransformComp->Position;
	float Distance = D3DXVec3Length(&GoalTargetDiff);

	if (GoalPos == vec3{ 0.f,0.f,0.f })
	{
		Paths = AStarManager::instance().PathFind(_PhysicComp->Position, _Target->_TransformComp->Position);
		if (!Paths.empty())
		{
			FollowRouteProcedure();
		}
		return;
	}
	else if (Distance > PathFindCheckMinDistanceMin &&
    abs( GoalPos.x - _Target->_TransformComp->Position .x )  > 100.f )  
	{
		Paths = AStarManager::instance().PathFind(_PhysicComp->Position, _Target->_TransformComp->Position);
		if (!Paths.empty())
		{
			FollowRouteProcedure();
		}
		return;
	}

	if (Paths.empty())return;

	vec3 ToMoveMark = CurMoveMark - _PhysicComp->Position;
	if (std::abs(ToMoveMark.x)< NextPathCheckMinDistance)
	{
		FollowRouteProcedure();
	}
	
	Move(_PhysicComp->Dir, Speed);
}

void Grunt::Turn()
{
	_CurrentState = Grunt::State::Turn;
	RenderComponent::NotifyType _Notify;
	_Notify[8] = [this]() {
		bTurnMotionEnd = true;
	};
	_RenderComp->Anim(true, false, L"spr_grunt_turn", 8, 0.45f,std::move(_Notify));
}

void Grunt::TurnState()
{
	if (bTurnMotionEnd)
	{
		bTurnMotionEnd = false;

		switch (_EnemyState)
		{
		case NormalEnemy::State::Idle:
			Idle();
			break;
		case NormalEnemy::State::Detecting:
			Run();
			break;
		case NormalEnemy::State::Walk:
			Walk();
			break;
		default:
			Idle();
			break;
		}
	}
}

void Grunt::Walk()
{
	_EnemyState = NormalEnemy::State::Walk;
	_CurrentState = Grunt::State::Walk;
	_RenderComp->Anim(false, true, L"spr_grunt_walk",
		10, 0.68f);
}

void Grunt::WalkState()
{
	vec3 BeforeDir = _PhysicComp->Dir;
	IsDetected = IsRangeInnerTarget();
	

	if (IsDetected)
	{
		if (D3DXVec3Dot(&BeforeDir, &ToTarget) < 0.f)
		{
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			Turn();
			return;
		}
		else
		{
			Run();
		}
	}

	Move(_PhysicComp->Dir, Speed *0.25f);
}


void Grunt::AnyState()
{

}

void Grunt::FSM()
{
	switch (_CurrentState)
	{
	case Grunt::State::Attack:
		AttackState();
		break;
	case Grunt::State::Fall:
		FallState();
		break;
	case Grunt::State::HurtFly:
		HurtFlyState();
		break;
	case Grunt::State::HurtGround:
		HurtGroundState();
		break;
	case Grunt::State::Idle:
		IdleState();
		break;
	case Grunt::State::Run:
		RunState();
		break;
	case Grunt::State::Turn:
		TurnState();
		break;
	case Grunt::State::Walk:
		WalkState();
		break;
	case Grunt::State::LeaveStair:
		LeaveStairState();
		break;
	case Grunt::State::EnterStair:
		EnterStairState();
		break;
	default:
		break;
	}

	AnyState();
}
void Grunt::FollowRouteProcedure()
{
	CurMoveMark = Paths.back();
	GoalPos = Paths.front();
	Paths.pop_back();

	vec3 ToPath = CurMoveMark - _PhysicComp->Position;
	_Y = ToPath.y;
	if (ToPath.y > 40.f)
	{
		_CollisionComp->bDownJump = true;
	}
	else
	{
		_CollisionComp->bDownJump = false;
	}

	if (D3DXVec3Dot(&ConvertXAxisDir(_PhysicComp->Dir), &ConvertXAxisDir(ToPath)) < 0.f)
	{
		_PhysicComp->Dir = ConvertXAxisDir(ToPath);
		Turn();
		return;
	}

	if (std::abs(ToPath.y) > DoorPathCheckMinDistance)
	{
		LeaveDoorLocation = CurMoveMark;
		LeaveDoorLocation.y -= 30.f;
		EnterStair();
		return;
	}
	else
	{
		_PhysicComp->Dir = ConvertXAxisDir(ToPath);
	}
};

void Grunt::SetUpInitState(float DirX, int32_t StateID)
{
	// TODO :: 여기서 초기 상태 지정 ..
	_RenderComp->Anim(false, true, L"spr_grunt_idle",
		8, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, L"Grunt", LAYER::ELAYER::EOBJECT);

	switch (StateID)
	{
	case 1:
		Idle();
		break;
	case 2:
		Walk();
		break;
	case 3:
		_RenderComp->Anim(false, true, L"spr_grunt_lean",
			1, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, L"Grunt", LAYER::ELAYER::EOBJECT);
		// 게으른 순찰상태.
		bLaziness = true;
		break;
	default:
		Idle();
		break;
	};

	// TOOD :: 여기서 초기 방향 지정 ..... 
	AtTheAttackDir = _PhysicComp->Dir = vec3{ DirX,0.f,0.f };
}
