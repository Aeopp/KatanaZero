#include "stdafx.h"
#include "Grunt.h"

#include "Player.h"
#include "Grunt_Slash.h"
#include "CollisionComponent.h"
#include "PhysicTransformComponent.h"
#include "RenderComponent.h"
#include "ComponentManager.h"
#include "ObjectManager.h"
#include "Time.h"

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
	_RenderComp->Anim(false, true, L"spr_grunt_idle",
		8, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, L"Grunt",
		LAYER::ELAYER::EOBJECT);
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

	_PhysicComp->Dir = vec3{ 1.f,0.f,0.f };
	PursuitRange = 700.f;
	NarrowRange = 180.f;

	bWalk = true;

	_SpAttack = ObjectManager::instance().InsertObject < Grunt_Slash>();
	_SpAttack->SetOwner(_This);


}

void Grunt::Update()
{
	Super::Update();

	FSM();
}

void Grunt::LateUpdate()
{
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
}

void Grunt::Attack()
{
	constexpr float AttackRich = 20.f;

	_CurrentState = Grunt::State::Attack;
	RenderComponent::NotifyType _Notify;
	_Notify[8] = [this]()
	{
		bAttackMotionEnd = true;
		_SpAttack->AttackStart(ToTargetDir * AttackRich, ToTargetDir);
	};
	_RenderComp->Anim(false, false, L"spr_grunt_attack", 8, 0.6f, std::move(_Notify));
	
};

void Grunt::AttackState()
{
	if (bAttackMotionEnd)
	{
		bAttackMotionEnd = false;
		Idle();
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
	
	_RenderComp->Anim(false, false, L"spr_grunt_hurtfly", 2, 0.2f);
	_PhysicComp->Flying();
}

void Grunt::HurtFlyState()
{
	if (_PhysicComp->bLand)
	{
		HurtGround();
	}
}

void Grunt::HurtGround()
{
	_CurrentState = Grunt::State::HurtGround;
	RenderComponent::NotifyType _Notify;
	_Notify[16] = [this]()
	{
		//DIE !!
	};
	_RenderComp->Anim(false, false, L"spr_grunt_hurtground",
		16, 1.f, std::move(_Notify));
}

void Grunt::HurtGroundState()
{

}

void Grunt::Idle()
{
	_CurrentState = Grunt::State::Idle;
	_RenderComp->Anim(false, true,	L"spr_grunt_idle", 8, 0.5f);
}

void Grunt::IdleState()
{
	IsDetected = IsRangeInnerTarget();

	if (IsDetected)
	{
		Run();
	}
}

void Grunt::Run()
{
	_CurrentState = Grunt::State::Run;
	_RenderComp->Anim(false, true, L"spr_grunt_run", 10, 0.6f);
}

void Grunt::RunState()
{
	bool IsAttackRange= IsRangeInnerTarget();

	// 추격중.........
	if (IsRangeInnerTarget())
	{
		if (ToTargetDistance < AttackRange)
		{
			Attack();
		}
	}
	// 추격중일때의 감지범위 바깥으로 타겟이 도망감
	else
	{
		if (bWalk)
		{
			Walk();

		}
		else
		{
			Idle();
		}
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
	_RenderComp->Anim(false, false, L"spr_grunt_turn", 8, 0.45f,std::move(_Notify));
}

void Grunt::TurnState()
{
	if (bTurnMotionEnd)
	{
		bTurnMotionEnd = false;

		switch (_CurState)
		{
		case NormalEnemy::State::Idle:
			Idle();
			break;
		case NormalEnemy::State::Detecting:
			Run();
			break;
		default:
			Idle();
			break;
		}
	}
}

void Grunt::Walk()
{
	_CurrentState = Grunt::State::Walk;
	_RenderComp->Anim(false, true, L"spr_grunt_walk",
		10, 0.6f);
}

void Grunt::WalkState()
{
	IsDetected = IsRangeInnerTarget();

	if (IsDetected)
	{
		Run();
	}
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
	default:
		break;
	}

	AnyState();
}
