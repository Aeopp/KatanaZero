#include "stdafx.h"
#include "Gangster.h"


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

OBJECT_ID::EID Gangster::GetID()
{
    return OBJECT_ID::EID::GANGSTER;
}

OBJECT_TAG::ETAG Gangster::GetTag()
{
    return Super::GetTag();
}

std::wstring_view Gangster::GetName() const&
{
    return L"Gangster"sv;

}

void Gangster::Initialize() & noexcept
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

	Speed = 500.f;
	MoveGoalTime = 2.f;
	DetectionRange = 400.f;
	AttackRange = 500.f;

	_CurrentState = Gangster::State::Idle;

	PursuitRange = 700.f;
	NarrowRange = 180.f;
	//TODO::
	/*_SpAttack = ObjectManager::instance().InsertObject < Grunt_Slash>();
	_SpAttack->SetOwner(_This);*/
	DelayAfterAttack = 0.4f;

}

void Gangster::Update()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;

	Super::Update();

	FSM();
}

void Gangster::LateUpdate()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;


	Super::LateUpdate();

	const float Dt = Time::instance().Delta();
}

void Gangster::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CurrentState == Gangster::State::Walk && _CollisionInfo._ID == OBJECT_ID::ETILE)
	{
		if (
			(math::almost_equal(vec3{ 1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &vec3{ 1.f,0.f,0.f }) > cosf(math::PI / 5.f))) ||
			(math::almost_equal(vec3{ -1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &vec3{ -1.f,0.f,0.f }) > cosf(math::PI / 5.f)))
			)
		{
			Turn();
			_PhysicComp->Dir.x *= -1.f;
		}
	}
}

void Gangster::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target, _CollisionInfo);


}

void Gangster::Move(vec3 Dir, const float AddSpeed)
{
	Super::Move(Dir, AddSpeed);

}

void Gangster::Die()&
{
	HurtFly();

	bBlooding = true;

}

void Gangster::EnterStair()
{
	_CurrentState = Gangster::State::EnterStair;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]()
	{
		bEnterStairMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_gangster_enterstair", 6, 0.66, std::move(_Notify));
}

void Gangster::LeaveStair()
{
	_CurrentState = Gangster::State::LeaveStair;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]()
	{
		bLeaveStairMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_gangster_leavestair", 6, 0.66f, std::move(_Notify));
}

void Gangster::EnterStairState()
{
	if (bEnterStairMotionEnd)
	{
		bEnterStairMotionEnd = false;
		_PhysicComp->Position = LeaveDoorLocation;
		LeaveStair();
	}
}

void Gangster::LeaveStairState()
{
	if (bLeaveStairMotionEnd)
	{
		bLeaveStairMotionEnd = false;
		Run();
	}
}

void Gangster::Attack()
{
	constexpr float AttackRich = 20.f;

	_CurrentState = Gangster::State::Attack;
	RenderComponent::NotifyType _Notify;
	vec3 ToTarget = _Target->_TransformComp->Position;
	ToTarget -= _PhysicComp->Position;
	D3DXVec3Normalize(&ToTarget, &ToTarget);

	// X축 방향만 필요하다면 사용 아니라면 사용 X ToTarget = ConvertXAxisDir(ToTarget);
	//TODO :: 
	//_Notify[4] = [ToTarget, this]()
	//{
	//	bAttackMotionEnd = true;
	//	//TODO :: 
	//	//_SpAttack->AttackStart(ToTarget * AttackRich, ToTarget);
	//};
	//_RenderComp->Anim(false, false, L"spr_grunt_attack", 8, 0.6f, std::move(_Notify));
	AtTheAttackDir = _PhysicComp->Dir;
}

void Gangster::AttackState()
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

void Gangster::Fall()
{
}

void Gangster::FallState()
{
}

void Gangster::HurtFly()
{
	_CurrentState = Gangster::State::HurtFly;
	_RenderComp->Anim(false, false, L"spr_gangsterhurtfly", 2, 0.5f);
	_PhysicComp->Flying();
}

void Gangster::HurtFlyState()
{
	if (_PhysicComp->bLand)
	{
		bBlooding = false;
		HurtGround();
	}
}

void Gangster::HurtGround()
{
	_CurrentState = Gangster::State::HurtGround;
	RenderComponent::NotifyType _Notify;
	bBloodingOverHead = true;

	_Notify[14] = [this]()
	{
		_RenderComp->bSlowRender = true;
		bHurtGroundMotionEnd = true;
	};

	_Notify[8] = [this]() {bBloodingOverHead = false; };

	_RenderComp->Anim(false, false, L"spr_gangsterhurtground",
		14, 1.8f, std::move(_Notify));
}

void Gangster::HurtGroundState()
{
	if (bHurtGroundMotionEnd)
	{
		bHurtGroundMotionEnd = false;
	}
}

void Gangster::Idle()
{
	_CurrentState = Gangster::State::Idle;
	_RenderComp->Anim(false, true, L"spr_gangsteridle", 8, 0.5f);
}

void Gangster::IdleState()
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

void Gangster::Run()
{
	if (!global::IsPlay())return;

	_CurrentState = Gangster::State::Run;
	_RenderComp->Anim(false, true, L"spr_gangsterrun", 10, 0.6f);
	bLaziness = false;
}

void Gangster::RunState()
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
		abs(GoalPos.x - _Target->_TransformComp->Position.x) > 100.f)
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
	if (std::abs(ToMoveMark.x) < NextPathCheckMinDistance)
	{
		FollowRouteProcedure();
	}

	Move(_PhysicComp->Dir, Speed);
}

void Gangster::Turn()
{
	_CurrentState = Gangster::State::Turn;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]() {
		bTurnMotionEnd = true;
	};
	_RenderComp->Anim(true, false, L"spr_gangsterturn", 6, 0.53f, std::move(_Notify));
}

void Gangster::TurnState()
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

void Gangster::Walk()
{
	_EnemyState = NormalEnemy::State::Walk;
	_CurrentState = Gangster::State::Walk;
	_RenderComp->Anim(false, true, L"spr_gangsterwalk",
		8, 0.84f);
}

void Gangster::WalkState()
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

	Move(_PhysicComp->Dir, Speed * 0.25f);
}

void Gangster::AnyState()
{
}

void Gangster::FSM()
{
	switch (_CurrentState)
	{
	case Gangster::State::Attack:
		AttackState();
		break;
	case Gangster::State::Fall:
		FallState();
		break;
	case Gangster::State::HurtFly:
		HurtFlyState();
		break;
	case Gangster::State::HurtGround:
		HurtGroundState();
		break;
	case Gangster::State::Idle:
		IdleState();
		break;
	case Gangster::State::Run:
		RunState();
		break;
	case Gangster::State::Turn:
		TurnState();
		break;
	case Gangster::State::Walk:
		WalkState();
		break;
	case Gangster::State::LeaveStair:
		LeaveStairState();
		break;
	case Gangster::State::EnterStair:
		EnterStairState();
		break;
	default:
		break;
	}

	AnyState();
}

void Gangster::FollowRouteProcedure()
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
}

void Gangster::SetUpInitState(float DirX, int32_t StateID)
{	// TODO :: 여기서 초기 상태 지정 ..
	_RenderComp->Anim(false, true, L"spr_gangsteridle",
		8, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, 
	L"Gangster", LAYER::ELAYER::EOBJECT);

	switch (StateID)
	{
	case 0:
		Idle();
		/*_RenderComp->Anim(false, true, L"spr_grunt_lean",
			1, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, L"Grunt", LAYER::ELAYER::EOBJECT);*/
		// 게으른 순찰상태.
		bLaziness = true;
		break;
	case 1:
		Idle();
		break;
	case 2:
		Walk();
		break;
	default:
		Idle();
		break;
	};

	// TOOD :: 여기서 초기 방향 지정 ..... 
	AtTheAttackDir = _PhysicComp->Dir = vec3{ DirX,0.f,0.f };
};
