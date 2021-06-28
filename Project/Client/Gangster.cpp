#include "stdafx.h"
#include "Gangster.h"


#include "RenderComponent.h"
#include "EffectManager.h"
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
#include "Component.h"
#include "Texture_Manager.h"
#include "GangsterGun.h"
#include "GangsterArm.h"
#include "Door.h"

#include "sound_mgr.h"

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
	_RenderComp->PositionCorrection = vec3{ 0.f,-20.f,0.f };

	_CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 35;
	_CollisionComp->_CollisionInfo.Width = 18;

	_CollisionComp->bDownJump = false;

	_PhysicComp->bGravity = true;

	Speed = 633.f;
	MoveGoalTime = 2.f;
	DetectionRange = 1100.f;
	AttackRange = 870.f;

	_CurrentState = Gangster::State::Idle;

	PursuitRange = 700.f;
	NarrowRange = 500.f;
	//TODO::
	/*_SpAttack = ObjectManager::instance().InsertObject < Grunt_Slash>();
	_SpAttack->SetOwner(_This);*/
	DelayAfterAttack = 0.66f;

	_SpGun = ObjectManager::instance().InsertObject<GangsterGun>();
	_SpGun->SetOwner(_This);

	_SpArm = ObjectManager::instance().InsertObject<GangsterArm>();
	_SpArm->SetOwner(_This);

	//IsSamefloorRange = { -169.f,+60.f};
	IsSamefloorRange = { -250.f,+120.f};

	DetectionRange = AttackRange;
}

void Gangster::Update()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;

	Super::Update();

	// _SpGun->_TransformComp->Rotation.z = GunRotZ;
	 // _SpGun->_RenderComp->AnimDir = _RenderComp->AnimDir;

	FSM();
}

void Gangster::LateUpdate()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;
	
	if (_EnemyState == NormalEnemy::State::Die)
	{
		_SpArm->_RenderComp->bRender = false;
		_SpGun->_RenderComp->bRender = false;
	
	}

	Super::LateUpdate();

	const float Dt = Time::instance().Delta();
	DoorTurnDuration -= Dt;
	AttackCoolTime -= Dt;
	CurRunSoundTime -=Dt;	
	CurWalkSoundTime -= Dt;
}

void Gangster::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CurrentState == Gangster::State::Walk && _CollisionInfo._ID == OBJECT_ID::ETILE)
	{
			const vec3 RightVector{ 1.f,0.f,0.f };   
			const vec3 LeftVector{ -1.f,0.f,0.f };
		if (
			(math::almost_equal(vec3{ 1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &RightVector ) > cosf(math::PI / 3.5f))) ||
			(math::almost_equal(vec3{ -1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &LeftVector) > cosf(math::PI / 3.5f)))
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

	if (_EnemyState == NormalEnemy::State::Die)return;

	if ((_EnemyState == NormalEnemy::State::Walk ||
		_CurrentState == Gangster::State::Walk) && _CollisionInfo._ID == OBJECT_ID::DOOR &&
		DoorTurnDuration < 0.f)
	{
		auto spObject = _CollisionInfo._Target.lock();
		auto spDoor = std::dynamic_pointer_cast<Door>(spObject);

		if (!spDoor->bOpening)
		{
			DoorTurnDuration = 1.f;
			Turn();
			_PhysicComp->Dir.x *= -1.f;
		}
	}

	if (_CollisionInfo._ID == OBJECT_ID::SMOKE_CLOUD && _CurrentState != Gangster::State::InSmoke)
	{
		InSmoke();
	}

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
	_RenderComp->Anim(false, false, L"spr_gangsterenterstair", 6, 0.66, std::move(_Notify));
}

void Gangster::LeaveStair()
{
	_CurrentState = Gangster::State::LeaveStair;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]()
	{
		bLeaveStairMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_gangsterleavestair", 6, 0.66f, std::move(_Notify));
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
	_RenderComp->PositionCorrection = vec3{ 0.f,-20.f,0.f };
	AttackCoolTime = 0.6f;

	_CurrentState = Gangster::State::Attack;
	RenderComponent::NotifyType _Notify;
	vec3 ToTarget = _Target->_TransformComp->Position - _PhysicComp->Position;
	D3DXVec3Normalize(&ToTarget, &ToTarget);

	GunRotZ = atan2f(ToTarget.y, ToTarget.x);
	// ¿©±â¼­ ÃÑ ·»´õ¸µ ÄÒ´Ù
	_SpGun->GunRenderStart();
	_SpArm->ArmRenderStart();

	_RenderComp->Anim(false, false, L"spr_gangsteraim", 7, 0.1f);
	AtTheAttackDir = math::ConvertXAxisDir(ToTarget);
}

void Gangster::AttackState()
{
	if (_EnemyState == NormalEnemy::State::Die)return;

	if (_Target->bInAreaSmoke && _CurrentState != Gangster::State::InSmoke)
	{
		InSmoke();
		_SpGun->GunRenderEnd();
		_SpArm->ArmRenderEnd();

		return;
	}

	vec3 ToTarget = _Target->_PhysicComp->Position - _PhysicComp->Position;
	float TargetDistance = D3DXVec3Length(&ToTarget);

	if (D3DXVec3Dot(&AtTheAttackDir, &ToTarget) < 0.f)
	{
		_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
		_SpGun->GunRenderEnd();
		_SpArm->ArmRenderEnd();

		// ¿©±â¼­ ÃÑ ·»´õ¸µ ²ö´Ù
		Turn();
		return;
	};

	if (D3DXVec3Length(&ToTarget) < 80.f)
	{
		if ((!_Target->IsInvisible()) && (!_Target->bHurt))
		{
			_SpGun->GunRenderEnd();
			// ¿©±â¼­ ÃÑ ·»´õ¸µ ²ö´Ù
			_SpArm->ArmRenderEnd();
			Whip();
			return;
		};
	};
	
	if (TargetDistance < AttackRange && IsSamefloor(_Target->_TransformComp->Position,false))
	{
		if (AttackCoolTime < 0.f)
		{
			AttackCoolTime = 0.66f;
			constexpr float AttackRich = 70.f;
			constexpr float BulletSpeed = 1800.f;

			RAND_SOUNDPLAY("gun_fire", { 1,2 }, 0.8f);
			RAND_SOUNDPLAY("sound_enemy_fire", { 1,2 }, 1);
			
			vec3 ToTarget = _Target->_TransformComp->Position;
			ToTarget -= _PhysicComp->Position;
			D3DXVec3Normalize(&ToTarget, &ToTarget);
			vec3 FireLocation = _PhysicComp->Position + (ToTarget * AttackRich);

			int32_t FireEffectImgID = math::Rand<int32_t>({ 0,2 });
			std::wstring FireEftKey = L"spr_fire_" + std::to_wstring(FireEffectImgID);
			std::wstring SparkEftKey = L"Gunspark" + std::to_wstring(FireEffectImgID);

			EffectManager::instance().EffectPush(L"Effect", L"spr_bullet",
				1, (std::numeric_limits<float>::max)(), 10.f, OBJECT_ID::EID::BULLET,
				true, FireLocation, ToTarget * BulletSpeed, { 1,1,1 }, false,
				true, false, true, 34 , 2, 255, false, 0.f, atan2f(ToTarget.y, ToTarget.x),
				0);

			EffectManager::instance().EffectPush(L"Effect", FireEftKey, 6, 0.1f, 0.1f * 6 + 0.01f,
				OBJECT_ID::EID::ENONE, true, FireLocation, { 0,0,0 }, { 2,2,2, },
				false, false, false, false, 0, 0,
				255, false, 0,
				atan2f(ToTarget.y, ToTarget.x));

			EffectManager::instance().EffectPush(L"Effect", SparkEftKey, 8, 0.05f,
				8 * 0.05f + 0.01f, OBJECT_ID::EID::ENONE, true, FireLocation, { 0,0,0 },
				{ 2,2,1 }, false, false, false, false, 0, 0, 255, false, 0,
				atan2f(ToTarget.y, ToTarget.x));
			std::wstring GunSmokeKey = L"spr_gunsmoke" + std::to_wstring(FireEffectImgID);

			EffectManager::instance().EffectPush(L"GunSmoke", GunSmokeKey, 10,
				0.1f, 10 * 0.1f + 0.01f, OBJECT_ID::EID::ENONE, true, FireLocation, { 0,0,0 },
				{ 2,2,2 }, false, false, false, false, 0, 0, 255, false, 0, atan2f(ToTarget.y, ToTarget.x),
				0, 0);
		}
	}
	else
	{
		Time::instance().TimerRegist(DelayAfterAttack, DelayAfterAttack, DelayAfterAttack,
			[this,This=_This]() {

			if (This.expired())return true ;
			if (!global::IsPlay())return true;
			if (_EnemyState != NormalEnemy::State::Die)
			{
				_SpGun->GunRenderEnd();
				// ¿©±â¼­ ÃÑ ·»´õ¸µ ²ö´Ù
				_SpArm->ArmRenderEnd();
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
	const float dt = Time::instance().Delta();
	HurtFlyTime += dt;

	if (_PhysicComp->bLand || HurtFlyTime >= HurtFlyLimitTime)
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

	_RenderComp->PositionCorrection = { 0,0,0 };

	SOUNDPLAY("sound_head_bloodspurt", 1 );

}

void Gangster::HurtGroundState()
{
	if (bHurtGroundMotionEnd)
	{
		bHurtGroundMotionEnd = false;
		if (!bDeadHeadSound)
		{
			bDeadHeadSound = true;
			RAND_SOUNDPLAY("sound_head", { 1,2 }, 0.65f);
		}
		
	}
}

void Gangster::Idle()
{
	_CurrentState = Gangster::State::Idle;
	_RenderComp->Anim(false, true, L"spr_gangsteridle", 8, 0.7f);
}

void Gangster::IdleState()
{
	vec3 BeforeDir = _PhysicComp->Dir;
	IsDetected = IsRangeInnerTarget();

	if (IsDetected)
	{
		if (_Target->bInAreaSmoke && _CurrentState != Gangster::State::InSmoke)
		{
			InSmoke();
			return;
		}
		else if (D3DXVec3Dot(&BeforeDir, &ToTarget) < 0.f)
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

	_RenderComp->PositionCorrection = vec3{ 0.f,-5.f,0.f };
}

void Gangster::RunState()
{
	vec3 ToTarget = _Target->_TransformComp->Position - _PhysicComp->Position;
	float ToTargetDistance = D3DXVec3Length(&ToTarget);

	if (_Target->bInAreaSmoke && _CurrentState != Gangster::State::InSmoke &&
		ToTargetDistance < PursuitRange)
	{
		InSmoke();
		return;
	}

	if (ToTargetDistance < AttackRange)
	{
		if (IsSamefloor(_Target->_TransformComp->Position,false ))
		{
			Attack();
			return;
		}
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

	if (CurRunSoundTime < 0.f)
	{
		CurRunSoundTime = RunSoundTime;
		RAND_SOUNDPLAY("sound_generic_enemy_run", { 1,4 }, 0.6f);
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
	_RenderComp->PositionCorrection = { 0,-10.f,0 };
	_RenderComp->Anim(false, false, L"spr_gangsterturn", 6, 0.3f, std::move(_Notify));
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
	_RenderComp->PositionCorrection += {0,0,0};
}

void Gangster::WalkState()
{
	vec3 BeforeDir = _PhysicComp->Dir;
	IsDetected = IsRangeInnerTarget();

	if (IsDetected)
	{
		if (_Target->bInAreaSmoke && _CurrentState != Gangster::State::InSmoke)
		{
			InSmoke();
			return;
		}
		else if (D3DXVec3Dot(&BeforeDir, &ToTarget) < 0.f)
		{
			_RenderComp->PositionCorrection += {0, 0, 0};
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			Turn();
			return;
		}
		else
		{
			_RenderComp->PositionCorrection += {0, 0, 0};
			Run();
		}
	}

	if (CurWalkSoundTime < 0.f)
	{
		CurWalkSoundTime = WalkSoundTime;
		RAND_SOUNDPLAY("sound_generic_enemy_walk", { 1,4 }, 0.6f);
	}


	Move(_PhysicComp->Dir, Speed * 0.25f);
}

void Gangster::Whip()
{
	_CurrentState = Gangster::State::Whip;
	RenderComponent::NotifyType _Notify;
	_RenderComp->PositionCorrection = {0,0,0};

	_Notify[0] = [this]()
	{
		if (!_Target->IsInvisible() && !_Target->bHurt)
		{
			_Target->HurtGround();
			_Target->_PhysicComp->Move(_PhysicComp->Dir * 900.f, 30.f, 0.4f, _PhysicComp->Dir, 0);
		}
	};

	_Notify[6] = [this]() 
	{
		bWhipMotionEnd = true;
	};

	_RenderComp->Anim(false,false,L"spr_gangsterwhip",
		6, 0.65f, std::move(_Notify));
	
	 // vec3 TargetLocation = _Target->_TransformComp->Position;
}

void Gangster::WhipState()
{
	if (bWhipMotionEnd)
	{
		bWhipMotionEnd = false;
		Attack();
		return;
	}
}

void Gangster::InSmoke()
{
	if (_CurrentState == Gangster::State::Idle)
	{
	//	_RenderComp->PositionCorrection += { 0, -10, 0 };
	}
	_EnemyState = NormalEnemy::State::Idle;
	_CurrentState = Gangster::State::InSmoke;
	_SpGun->GunRenderEnd();
	_SpArm->ArmRenderEnd();

	_RenderComp->Anim(true, true, L"spr_gangsteridle",8, 0.7f);
	//_RenderComp->PositionCorrection += { 0, 10, 0 };

	_MsgRenderComp->bRender = true;
	_MsgRenderComp->Anim(true, false, L"quest",
		1, 1.f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"spr_enemy_question",
		LAYER::ELAYER::EOBJECT_OVER);

	Time::instance().TimerRegist(0.3f, 0.3f, 0.3f, [this]() {
		bSmokeEnd = true;
		return true; });
}

void Gangster::InSmokeState()
{
	if (bSmokeEnd)
	{
		//_RenderComp->PositionCorrection += { 0, -10, 0 };
		bSmokeEnd = false;
		Idle();
	}

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
	case Gangster::State::Whip:
		WhipState();
		break; 
	case Gangster::State::InSmoke:
		InSmokeState();
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
	const auto PhysicCompDir = ConvertXAxisDir(_PhysicComp->Dir);
	const auto  ToPathVector = ConvertXAxisDir(ToPath); 

	if (D3DXVec3Dot(&PhysicCompDir, &ToPathVector) < 0.f)
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
{	// TODO :: ¿©±â¼­ ÃÊ±â »óÅÂ ÁöÁ¤ ..
	_RenderComp->Anim(false, true, L"spr_gangsteridle",
		8, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, 
	L"Gangster", LAYER::ELAYER::EOBJECT);

	switch (StateID)
	{
	case 1:
		Idle();
		/*_RenderComp->Anim(false, true, L"spr_grunt_lean",
			1, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, L"Grunt", LAYER::ELAYER::EOBJECT);*/
			// °ÔÀ¸¸¥ ¼øÂû»óÅÂ.
		break;
	case 2:
		Walk();
		break;
	case 3:
		Idle();
		bLaziness = true;
		break;
	default:
		Idle();
		break;
	};

	// TOOD :: ¿©±â¼­ ÃÊ±â ¹æÇâ ÁöÁ¤ ..... 
	AtTheAttackDir = _PhysicComp->Dir = vec3{ DirX,0.f,0.f };
};
