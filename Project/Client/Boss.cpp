#include "stdafx.h"
#include "Boss.h"
#include "Item.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "Time.h"
#include "NormalEnemy.h"
#include "Player.h"
#include "CollisionComponent.h"
#include "RenderComponent.h"
#include "GraphicDevice.h"
#include "PhysicTransformComponent.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "Door.h"
#include "Grenade.h"

OBJECT_ID::EID Boss::GetID()
{
	return OBJECT_ID::EID::BOSS;
}

OBJECT_TAG::ETAG Boss::GetTag()
{
	return OBJECT_TAG::ETAG::EENEMY;
}

std::wstring_view Boss::GetName() const&
{
	return L"Boss"sv;
}

void Boss::Initialize() & noexcept
{
	Super::Initialize();

	_RenderComp->_RenderAfterEvent =[this]()
	{

	};

	_RenderComp->PlayGoalColor = D3DCOLOR_ARGB(0, 255, 0, 255);
	_RenderComp->PlayStartColor = D3DCOLOR_ARGB(125, 255, 0, 255);

	///*_RenderComp->SlowDeltaCoefft = 0.05f;
	//_RenderComp->AfterDeltaCoefft = 0.07f;*/

	//_RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	//_RenderComp->SlowColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	//_RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 0);

	_TransformComp->Scale *= 2.5f;
	_PhysicComp->Mass = 0.f;

	_RenderComp->bRender = true;

	_RenderComp->AfterImgOff();
	_RenderComp->PositionCorrection = vec3{ 0,-10,0 };

	_CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 35;
	_CollisionComp->_CollisionInfo.Width = 18;

  	_PhysicComp->bGravity = true;
	Speed = 1000.f;
	MoveGoalTime = 0.5f;

	UpdateBossDir();
	_BossState = Boss::State::Idle;
	RenderComponent::NotifyType _Notify;
	_Notify[12] = [this]() {};
	_RenderComp->Anim(false, true, L"spr_headhunter_idle", 12, 0.5f, std::move(_Notify),
		D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"HeadHunter", LAYER::ELAYER::EOBJECT);
	
	std::generate(std::begin(_Grenades), std::end(_Grenades),
		[]() {return ObjectManager::instance().InsertObject<Grenade>(); });

	_TransformComp->Dir = { -1,0,0 };

	Time::instance().TimerRegist(3, 3, 3,
		[this]() 
	{
		_TransformComp->Dir ={ -1,0,0 };
		PreDash();
		return true; 
	});
}

void Boss::LateUpdate()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;


	Super::LateUpdate();

	const float Dt = Time::instance().Delta();
	WallRideEffectDelta -= Dt;
	if (bWallJumpFire)
	{
		CurWallJumpFireDt -= Dt;
	}

	// -=dt;

	
}

void Boss::Update()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;
	
	Super::Update();

	FSM();
}

void Boss::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target, _CollisionInfo); 

	if (_BossState == Boss::State::Die || IsHurt())
	{
		return; 
	}

	if (_CollisionInfo._ID == OBJECT_ID::ATTACK_SLASH
		|| _CollisionInfo._ID == OBJECT_ID::DRAGON_DASH)
	{
		_PhysicComp->ForceClear();

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 3.f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		BloodInit(_CollisionInfo.PushDir);

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_slashfx", 5, 0.02f,
			0.02f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false, _PhysicComp->Position,
			{ 0,0,0 }, { 2.9,2.9,0 });

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

		EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);

		_RenderComp->AfterImgOn();

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce * 3, _CollisionInfo.PushDir, 0.2f);

		Time::instance().TimeScale = 0.2f;
		Time::instance().TimerRegist(0.05f, 0.05f, 0.05f, [this]() {

			if (global::ECurGameState::PlaySlow != global::_CurGameState)
			{
				Time::instance().TimeScale = 1.f;
			}
			return true; });

		_RenderComp->AfterImgOn();
		_RenderComp->NormalAfterImgPushDelta *= 1.f;

		Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]()
		{
			_RenderComp->AfterImgOff();
			_RenderComp->NormalAfterImgPushDelta *= 1.f;
			return true;
		});

		++CurHitCount;
		if (CurHitCount >= 5)
		{
			Die();
		}
		HurtFly();
	}
}

void Boss::MapHit(math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CollisionInfo._ID == OBJECT_ID::EWALLRIDELINE)
	{
		CurWallRideLinePos = _CollisionInfo.Position;
	}

	if ( ( _CollisionInfo._ID == OBJECT_ID::EWALLRIDELINE && _BossState == Boss::State::Jump) )
	{
		// 벽점프 아이들
		WallIdle();
	}

	if (_PhysicComp->Position.y >BossStateFloor && _BossState == Boss::State::WallJump)
	{
		_PhysicComp->bLand = true;
		_PhysicComp->Position.y = BossStateFloor - (35 * 2.5f) / 2.f;
		//WallJumpLand();
	}
}

int32_t Boss::AnimAimRifleID()
{
	vec3 TargetLocation = _Target->_TransformComp->Position;
	vec3 Location = _TransformComp->Position;
	vec3 ToTarget = TargetLocation - Location;
	vec3 ToTargetDir;
	D3DXVec3Normalize(&ToTargetDir, &ToTarget);
	ToTargetDir.y *= -1.f;
	ToTargetDir.x = std::abs(ToTargetDir.x);
	float Radian = atan2f(ToTargetDir.y, ToTargetDir.x);

	/*if (Radian < 0.f)
	{
		Radian = max(Radian, math::PI / -2.f);
	}
	else
	{
		Radian -= math::PI / 2.f;
	}*/

	int32_t Idx = 9;

	float PIDiff= (math::PI/2.f) / 9.f;

	Idx += (Radian / PIDiff);
	return Idx;
}

void Boss::AnyState()
{

}

void Boss::FSM()
{
	switch (_BossState)
	{
	case Boss::State::Idle:
		IdleState();
		break;
	case Boss::State::PreDash:
		PreDashState();
		break;
	case Boss::State::Dash:
		DashState();
		break;
	case Boss::State::DashEndGround:
		DashAndGroundState(); 
		break;
	case Boss::State::Die:
		DieState(); 
		break;
	case Boss::State::TakeOutRifle:
		TakeOutRifleState();
		break;
	case Boss::State::AimRifle:
		AimRifleState();
		break;
	case Boss::State::PreJump:
		PreJumpState();
		break;
	case Boss::State::Jump:
		JumpState();
		break;
	case Boss::State::WallIdle:
		WallIdleState();
		break;
	case Boss::State::WallJump:
		WallJumpState();
		break;
	case Boss::State::WallJumpLand:
		WallJumpLandState();
		break;
	case Boss::State::TakeOutGun:
		TakeOutGunState();
		break;
	case Boss::State::Shoot:
		ShootState();
		break;
	case Boss::State::Hurt:
		HurtState();
		break;
	case Boss::State::HurtFly:
		HurtFlyState();
		break;
	case Boss::State::PutBackGun:
		PutBackGunState();
		break;
	case Boss::State::PutBackRifle:
		PutBackRifleState();
		break;
	case Boss::State::Recover:
		RecoverState();
		break;
	default:
		break;
	}

	AnyState();
}

void Boss::PreDash()
{
	// UpdateAnimDir();
	UpdateBossDir();
	_BossState = Boss::State::PreDash;
	RenderComponent::NotifyType _Notify;
	constexpr float DashReadyTime = 0.5f;
	_Notify[8] = [this]()
	{
		bPreDashEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_predash",
		8, DashReadyTime, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255),
		0, { 1,1 }, L"HeadHunter", LAYER::ELAYER::EOBJECT);
	
	constexpr float ReadyNumber = 10;
	constexpr float ReadyDashDiff = 2000.f / ReadyNumber;
	constexpr float ReadyInitRich = 300.f;

	vec3 ToTargetDir = _PhysicComp->Dir;
	const vec3 InitScale = { 20,4.f,0 };
	vec3 GoalScale = { 0.f,0.f,0 };

	vec3 InitLocation = _PhysicComp->Position;
	InitLocation.y -= 18;

	for (int32_t i = 0; i < ReadyNumber; ++i)
	{
		vec3 CurInitScale = InitScale;
		CurInitScale.x = math::Rand<float>({ InitScale.x / 2.f,InitScale.x });
		vec3 ReadyLaserLocation = InitLocation + ToTargetDir *
			(i * ReadyDashDiff + ReadyInitRich);
		ReadyLaserLocation.y += math::Rand<float>({ -2,2 });

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_boss_dash_red", 1, FLT_MAX, DashReadyTime, OBJECT_ID::EID::ENONE,
			true, ReadyLaserLocation, { 0,0,0 }, CurInitScale, false, false, false, false, 0, 0,
			255, false, 0, atan2f(ToTargetDir.y, ToTargetDir.x), 0, 0, false, 0, 0, true, CurInitScale, GoalScale);
	};
}

void Boss::PreDashState()
{
	if (bPreDashEnd)
	{
		bPreDashEnd = false;
		Dash();
	}
}

void Boss::Idle()
{
	UpdateBossDir();
	_BossState = Boss::State::Idle;
	RenderComponent::NotifyType _Notify;
	_Notify[12] = [this]() {};
	_RenderComp->Anim(false, true, L"spr_headhunter_idle", 12, 1.2f, std::move(_Notify),
		D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"HeadHunter", LAYER::ELAYER::EOBJECT);
}

void Boss::IdleState()
{

}

void Boss::Dash()
{
	constexpr float DashTime = 0.4f;
	constexpr float DashSpeed = 3300.f;

	UpdateBossDir();

	_BossState = Boss::State::Dash;
	RenderComponent::NotifyType _Notify;
	_Notify[1] = [this]()
	{
		bDashEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_dash", 1, DashTime,
		std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"HeadHunter",
		LAYER::ELAYER::EOBJECT);
	
	_PhysicComp->Move(_TransformComp->Dir * DashSpeed, 0, DashTime, { 0,0,0 }, 0);
}

void Boss::DashState()
{
	if (bDashEnd)
	{
		bDashEnd = false;
		DashAndGround();
	}
	else
	{
		constexpr float EftTime = 1.f;
		constexpr float LerpDiff = 0.3f;
		constexpr float AfterAlpha = 30;

		/*constexpr float DashEftTime = 0.6f;
		constexpr float LerpDiff = 0.025f;
		constexpr float AfterAlpha = 7;*/

		// 현재와 과거 사이 위치를 선형보간해서 잔상 이펙트를 뿌린다.
		vec3 Past = _TransformComp->PastLocation;
		vec3 Cur = _TransformComp->Position;
		vec3 InBetWeen;
		int32_t i = 0;
		while (true)
		{
			const float CurLerp = LerpDiff * i;
			D3DXVec3Lerp(&InBetWeen, &Past, &Cur, CurLerp);
			if (CurLerp >= 1.f)break;
			EffectManager::instance().EffectPush(L"HeadHunter",
				L"spr_headhunter_dash",
				1, EftTime, EftTime, OBJECT_ID::EID::BOSS_DASH,
				false, InBetWeen, { 0,0,0 }, _PhysicComp->Scale,
				false, false, false, false, 0, 0, AfterAlpha, true, 0, 0, 0, 0, 0, 0, 0);

			++i;
		}
	}
}

void Boss::Die()&
{

}

void Boss::UpdateBossDir()
{
	/*vec3 TargetLocation  = _Target->_TransformComp->Position;
	vec3 Location = _TransformComp->Position;
	vec3 ToTarget = TargetLocation - Location;

	vec3 ToTargetDir;
	D3DXVec3Normalize(&ToTargetDir, &ToTarget);
	
	_TransformComp->Dir = ToTargetDir;*/
	if (_TransformComp->Position.x < BossStageCenterAxisX)
	{
		_TransformComp->Dir = { 1,0,0 };
	}
	else
	{
		_TransformComp->Dir = { -1,0,0 };
	}
	

	/*_RenderComp->AnimDir = 1.f;

	if (ToTarget.x < 0.f)
	{
		_RenderComp->AnimDir = 1.f;
		
	}*/
}


void Boss::DieState()
{
	
}

void Boss::DashAndGround()
{
	UpdateBossDir();

	_BossState = Boss::State::DashEndGround;
	RenderComponent::NotifyType _Notify;
	_Notify[10] = [this]()
	{
		bDashAndGroundEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_dash_end_ground",
		10, 0.6f, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255),
		0, { 1,1 }, L"HeadHunter", LAYER::ELAYER::EOBJECT);
};

void Boss::DashAndGroundState()
{
	if (bDashAndGroundEnd)
	{
		bDashAndGroundEnd = false;
		Dice(); 
		 // TakeOutRifle();
	}
}

void Boss::TakeOutRifle()
{
	UpdateBossDir(); ;
	
	_BossState = Boss::State::TakeOutRifle;
	RenderComponent::NotifyType _Notify;
	_Notify[8] = [this]()
	{
		bTakeOutRifleEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_takeoutrifle",
		8, 0.3f, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"HeadHunter",
		LAYER::ELAYER::EOBJECT);
}

void Boss::TakeOutRifleState()
{
	if (bTakeOutRifleEnd)
	{
		bTakeOutRifleEnd = false;
		AimRifle();
	}
}

void Boss::AimRifle()
{
	UpdateBossDir();

	_BossState = Boss::State::AimRifle;

	_RenderComp->Anim(true, false, L"spr_headhunter_aim_rifle",
		AnimAimRifleID() + 1, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
		AnimAimRifleID(), { 1,1 }, L"HeadHunter", LAYER::ELAYER::EOBJECT);

	constexpr float RifleLaserReadyTime = 1.f;
	constexpr float ReadyLaserNumber = 10;
	constexpr float ReadyLaserDiff = 2000.f/ ReadyLaserNumber;
	constexpr float ReadyLaserInitRich = 300.f;

	vec3 Location = _PhysicComp->Position;
	//vec3 TargetLocation = _Target->_PhysicComp->Position;
	//vec3 ToTarget = TargetLocation - Location;
	//vec3 ToTargetDir;
	vec3 ToTargetDir = _PhysicComp->Dir;
	const vec3 InitScale = { 20,1.25f,0 };
	vec3 GoalScale = { 0,0.5f,0 };
	//D3DXVec3Normalize(&ToTargetDir, &ToTarget);
	
	 vec3 InitLocation = Location;
	 InitLocation.y -= 18;

	for (int32_t i = 0; i < ReadyLaserNumber; ++i)
	{
		vec3 CurInitScale = InitScale;
		CurInitScale.x = math::Rand<float>({ InitScale.x / 2.f,InitScale.x });
		vec3 ReadyLaserLocation = InitLocation + ToTargetDir *
			(i * ReadyLaserDiff + ReadyLaserInitRich);
		ReadyLaserLocation.y += math::Rand<float>({ -2,2 });

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_bullet", 1, FLT_MAX, RifleLaserReadyTime, OBJECT_ID::EID::ENONE,
			true, ReadyLaserLocation, { 0,0,0 }, CurInitScale, false, false, false, false, 0, 0,
			255, false, 0, atan2f(ToTargetDir.y, ToTargetDir.x), 0, 0, false, 0, 0, true, CurInitScale, GoalScale);
	};

	Time::instance().TimerRegist(RifleLaserReadyTime, RifleLaserReadyTime, RifleLaserReadyTime,
		[this,InitLocation, ToTargetDir]()
	{
		if (_BossState != State::AimRifle)return true;

		vec3 LaserMiddle = InitLocation + ToTargetDir*(1024+50);
		vec3 StartScale{ 1,1,1 };
		vec3 GoalScale{ 1,1,1 };

		EffectManager::instance().EffectPush(L"HeadHunter", L"Laser",
			2, 0.2f, 0.5f, OBJECT_ID::EID::BOSS_LASER, true, LaserMiddle, { 0,0,0 },
			StartScale, true, true, false, false, 2048, 30, 255, false, 0, atan2f(ToTargetDir.y, ToTargetDir.x),
			0, 0, false, false, 0, true, StartScale, GoalScale);
			
		return true;
	});

	Time::instance().TimerRegist(RifleLaserReadyTime+ 0.5f, RifleLaserReadyTime+ 0.5f, RifleLaserReadyTime+ 0.5f,
		[this,InitLocation, ToTargetDir]()
	{
		if (_BossState != State::AimRifle)return true;

		vec3 LaserMiddle = InitLocation + ToTargetDir * (1024+50);
		vec3 StartScale{ 1,1,1 };
		vec3 GoalScale{ 1,0,1 };

		EffectManager::instance().EffectPush(L"HeadHunter", L"Laser",
			2, 0.2f, 0.5f, OBJECT_ID::EID::BOSS_LASER, true, LaserMiddle, { 0,0,0 },
			StartScale, true, false, false, false, 2048, 30, 255, false, 0, atan2f(ToTargetDir.y, ToTargetDir.x),
			0, 0, false, false, 0, true, StartScale, GoalScale);

		return true;
	});

	Time::instance().TimerRegist(2.f, 2.f, 2.f, [this]() {
		if (_BossState != State::AimRifle)return true;
		bAimRifleEnd = true;
		return true; });
}

void Boss::AimRifleState()
{
	if (bAimRifleEnd)
	{
		bAimRifleEnd = false;
		PutBackRifle();

	
		//PreJump();
	}
};

void Boss::PreJump()
{
	UpdateBossDir();
	_BossState = Boss::State::PreJump;
	RenderComponent::NotifyType _Notify;

	_Notify[3] = [this]()
	{
		bPreJumpEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_prejump", 3, 0.2f,
		std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255));
};

void Boss::PreJumpState()
{
	if (bPreJumpEnd)
	{
		bPreJumpEnd = false;
		Jump();
	}
};

void Boss::Jump()
{
	UpdateBossDir();
	_BossState = Boss::State::Jump;

	/*_PhysicComp->bFly = true;
	_PhysicComp->bLand = false;*/

	vec3 JumpDir = _TransformComp->Dir * -1.f;
	JumpDir = math::RotationVec(JumpDir, -70.f * JumpDir.x);
	constexpr float Force = 800.f;
	_PhysicComp->Move(JumpDir * Force, 0.f, FLT_MAX);

	_RenderComp->Anim(false, true, L"spr_headhunter_jump",1, 1.f, {});
};

void Boss::JumpState()
{
	//if(_PhysicComp->bwall)
	
	// IF 상태가 바뀌지 않는다면...
	{
		constexpr float EftTime = 0.8f;
		constexpr float LerpDiff = 3.f;
		constexpr float AfterAlpha = 140;

		// 현재와 과거 사이 위치를 선형보간해서 잔상 이펙트를 뿌린다.
		vec3 Past = _TransformComp->PastLocation;
		vec3 Cur = _TransformComp->Position;
		vec3 InBetWeen;
		int32_t i = 0;

		while (true)
		{
			const float CurLerp = LerpDiff * i;
			D3DXVec3Lerp(&InBetWeen, &Past, &Cur, CurLerp);
			if (CurLerp >= 1.f)break;
			EffectManager::instance().EffectPush(L"HeadHunter",
				L"spr_headhunter_jump",
				1, 1.f, EftTime, OBJECT_ID::EID::BOSS_JUMP,
				false, InBetWeen, { 0,0,0 }, _PhysicComp->Scale, false, false,
				false, false, 0, 0, AfterAlpha, true, 0, 0, 0, 0, false, false);

			++i;
		}
	}
};

void Boss::WallIdle()
{
	UpdateBossDir();
	_PhysicComp->ForceClear();
	_PhysicComp->GravityAcceleration = 0.f;

	_BossState = Boss::State::WallIdle;
	RenderComponent::NotifyType _Notify;
	_Notify[3] = [this]()
	{
		bWallIdleEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_wall_idle",
		3, 0.8f, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255));
};

void Boss::WallIdleState()
{
	if (bWallIdleEnd)
	{
		bWallIdleEnd = false;
		WallJump();
	}
	else
	{
		vec3 UpDir = { 0,-1,0 };
		D3DXVec3Lerp(&WallJumpDir, &_TransformComp->Dir, &UpDir, 0.5f);
		float factorX = _PhysicComp->Dir.x < 0.f ? +1 : -1;
		WallRideEffectDelta = 0.05f;
		vec3 Pos = _PhysicComp->Position + vec3{ math::Rand<float>({-1,+1}) * 20.f,50,0 };
		Pos.x += factorX * 20;
		vec3 Dir = math::RotationVec(WallJumpDir, math::Rand<float>({ -100,100 })) * 150;
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_dustcloud", 7, 0.06f, 7 * 0.06f + 0.01f, OBJECT_ID::EID::DustCloud, true, Pos, Dir, { 2.5,2.5,0 });
	}
};

void Boss::WallJump()
{
	UpdateBossDir();

	constexpr float AnimDt = 1.7f;
	constexpr float AnimNum = 7.f;
	constexpr float EndTime = AnimDt * AnimNum;
	CurWallJumpFireDt = -1.f;

	WallJumpFireDt = AnimDt * (4.f / 7.f) / 20.f;

	_BossState = Boss::State::WallJump;
	RenderComponent::NotifyType _Notify;
	_Notify[1] = [this]() {bWallJumpFire = true; };
	_Notify[5] = [this]() {bWallJumpFire = false; };
	_Notify[7] = [this]()
	{
		bWallJumpEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_walljump", AnimNum,
		AnimDt, std::move(_Notify));

	vec3 UpDir = { 0,-1,0 };

	D3DXVec3Lerp(&WallJumpDir, &_TransformComp->Dir, &UpDir, 0.6f);

	WallJumpT = 0.f;
	WallJumpInitPos = _PhysicComp->Position;

	_PhysicComp->bGravity = false;
	_PhysicComp->bLand = false;
	CurWallJumpFireAngle = 0.f;

	float factor = WallJumpDir.x < 0.f ? -1 : +1;
	vec3 Dir = WallJumpDir.x < 0.f ? vec3{ -1.f,0,0 } : vec3{ 1,0,0 };
	Dir *= 55.f;
	float RotZ = math::PI / 2.f * factor;

	EffectManager::instance().EffectPush(L"Effect", L"spr_jumpcloud", 4, 0.05f, 0.05f * 4 + 0.01f,
		OBJECT_ID::EID::JUMP_CLOUD, true, CurWallRideLinePos + Dir, { 0,0,0 }, { 2.5,2.5,2.5 }, false
		, false, false, false, 0, 0, 255, false, 0, RotZ);
}
void Boss::WallJumpEnd()
{
	bWallJumpEnd = false;
	_PhysicComp->bGravity = true;
}
;

void Boss::WallJumpState()
{
	if (_PhysicComp->bLand && bWallJumpEnd)
	{
		WallJumpLand();
	}
	else
	{
		// 포물선
		{
			const float Dt = Time::instance().Delta();
			constexpr float Force = 1600.f;
			constexpr float g = 850.f;
			WallJumpT += Dt;
			_PhysicComp->Position.x = WallJumpInitPos.x + (Force * WallJumpDir.x * WallJumpT);
			_PhysicComp->Position.y = WallJumpInitPos.y + (Force * WallJumpDir.y * WallJumpT)
				+
				((WallJumpT * WallJumpT) * g) * 0.5f;
		}

		// 잔상연출...
		{
			constexpr float EftTime = 0.8f;
			constexpr float LerpDiff = 3.f;
			constexpr float AfterAlpha = 140;

			// 현재와 과거 사이 위치를 선형보간해서 잔상 이펙트를 뿌린다.
			vec3 Past = _TransformComp->PastLocation;
			vec3 Cur = _TransformComp->Position;
			vec3 InBetWeen;
			int32_t i = 0;

			while (true)
			{
				const float CurLerp = LerpDiff * i;
				D3DXVec3Lerp(&InBetWeen, &Past, &Cur, CurLerp);
				if (CurLerp >= 1.f)break;
				EffectManager::instance().EffectPush(L"HeadHunter",
					L"spr_headhunter_walljump",
					1, EftTime, EftTime, OBJECT_ID::EID::BOSS_JUMP,
					false, InBetWeen, { 0,0,0 }, _PhysicComp->Scale,
					false, false, false, false, 0, 0, AfterAlpha, true, 0, 0, 0,
					_RenderComp->_Info.GetCurFrame(), 0, 0, 0, false);
				++i;
			}
		}

		if (bWallJumpFire && CurWallJumpFireDt < 0.f)
		{
			constexpr float BulletSpeed = 2000.f;
			vec3 BulletScale = { 4,1,0 };

			constexpr float FireRich = 100.f;
			CurWallJumpFireDt = WallJumpFireDt;
			vec3 FireDir = math::RotationVec(_TransformComp->Dir, D3DXToDegree(CurWallJumpFireAngle));
			vec3 InitLocation = _TransformComp->Position + FireDir * FireRich;
			float GunSparkScale = 2.5;

			for (int32_t i = 0; i < 3; ++i)
			{
				std::wstring StateKey = L"Gunspark" + std::to_wstring(i);
				EffectManager::instance().EffectPush(L"Effect", StateKey, 8,
					0.1f, 8 * 0.1f + 0.01f, OBJECT_ID::EID::ENONE, true, InitLocation,
					{ 0,0,0 }, { GunSparkScale,GunSparkScale,0 }, false, false, false, false, 0, 0,
					255, false, 0, atan2f(FireDir.y, FireDir.x), 0, 0, false, false, 0, false);
			}

			EffectManager::instance().EffectPush(L"Effect", L"spr_bullet", 1,
				1.f, 5.f, OBJECT_ID::EID::BULLET, true, InitLocation, FireDir * BulletSpeed,
				BulletScale, false, true, false, false, 34, 2, 255, false, 0,
				atan2f(FireDir.y, FireDir.x), 0, 0, false, false, 0);
			// 한발 쏘고나서

			float factor = 1.f;
			if (_TransformComp->Dir.x < 0.f)
			{
				factor = -1.f;
			}
			CurWallJumpFireAngle += (WallJumpFireAngleDiff * factor);
		}
	}
};

void Boss::WallJumpLandState()
{
	if (bWallJumpLandEnd)
	{
		bWallJumpLandEnd = false;
		Dice();
		//TakeOutGun();
		//........
	}
};

void Boss::WallJumpLand()
{
	WallJumpEnd();
	UpdateBossDir();
	_BossState = Boss::State::WallJumpLand;
	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]() {
		bWallJumpLandEnd = true;
	};
	_RenderComp->Anim(false,false,L"spr_headhunter_walljump_land",
		4, 0.3f, std::move(_Notify));
	_PhysicComp->ForceClear();
	UpdateBossDir();
}

void Boss::TakeOutGun()
{
	UpdateBossDir();
	_BossState = Boss::State::TakeOutGun;
	RenderComponent::NotifyType _Notify;

	_Notify[7] = [this]()
	{
		bTakeOutGunEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_takeoutgun",7, 0.8f, std::move(_Notify));
}

void Boss::TakeOutGunState()
{
	if (bTakeOutGunEnd)
	{
		bTakeOutGunEnd = false;
		Shoot();
	}
}

void Boss::Shoot()
{
	UpdateBossDir();
	CurShootCount = 0;
	bCurAnimLoopShoot = false;
	_BossState = Boss::State::Shoot;
	RenderComponent::NotifyType _Notify;

	_Notify[2] = [this]() 
	{	
		if (!bCurAnimLoopShoot)
		{
			bCurAnimLoopShoot = true;
			// 총을 쏜다.
			constexpr float Rich = 100.f;
			constexpr float InitSpeed = 2500.f;
			vec3 InitLocation = _TransformComp->Position + _TransformComp->Dir * Rich;
			InitLocation.y -= 20.f;

			vec3 LookDir = _TransformComp->Dir;
			vec3 Up = { 0,-1,0 };
			vec3 InitDir;
			D3DXVec3Lerp(&InitDir, &LookDir, &Up, math::Rand<float>({ 3,7 })*0.1f);
			D3DXVec3Normalize(&InitDir, &InitDir);

			_Grenades[CurShootCount]->Shoot(InitLocation, InitDir, InitSpeed);

			++CurShootCount;
		}
	};

	_Notify[8] = [this]() 
	{
		bCurAnimLoopShoot = false;

		if (CurShootCount >= GrenadeNum)
		{
			bShootEnd = true;
		}
	};

	_RenderComp->Anim(false, true, L"spr_headhunter_shoot", 8, 0.8f, std::move(_Notify),
		D3DCOLOR_ARGB(255, 255, 255, 255));
}

void Boss::ShootState()
{
	if (bShootEnd)
	{
		bShootEnd = false;
		CurShootCount = 0;
		PutBackGun();

		//Dice();
		//TakeOutRifle();
		//PreDash();
		//AimRifle();
	}
}

void Boss::Hurt()
{
	_RenderComp->PositionCorrection += vec3{ 0,10,0 };

	_BossState = Boss::State::Hurt;
	RenderComponent::NotifyType _Notify;
	_Notify[10] = [this]() {bHurtEnd = true; };
	_RenderComp->Anim(true, false, L"spr_headhunter_hurt", 10, 1.f, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255),
		2, { 1,1 });
}

void Boss::HurtState()
{
	if (bHurtEnd)
	{
		bHurtEnd = false;
		
		constexpr float CloudScale = 2.5f;
		vec3 Location = _TransformComp->Position;
		vec3 Floor = Location + vec3{ 0,25,0 };
		// 랜드 클라우드
		{
			EffectManager::instance().EffectPush(L"Effect", L"spr_landcloud",
				7, 0.125f, 7* 0.125f + 0.01f, OBJECT_ID::EID::ENONE, true, Floor, { 0,0,0 },
				{ CloudScale,CloudScale,0 }, false, false, false);
		}
		// 스톰퍼 클라우드
		const vec3 Range = { 70,0,0 };
		{
			EffectManager::instance().EffectPush(L"Effect", L"spr_stompercloud",
				10, 0.1f, 10 * 0.1f + 0.01f, OBJECT_ID::EID::ENONE, true, Floor + Range,
				{ 0,0,0 }, { CloudScale,CloudScale,0 }, false);

			EffectManager::instance().EffectPush(L"Effect", L"spr_stompercloud2",
				10, 0.1f, 10 * 0.1f + 0.01f, OBJECT_ID::EID::ENONE, true, Floor - Range,
				{ 0,0,0 }, { -CloudScale,CloudScale,0 }, false);
		}
		// 건스파크

		constexpr float GunSparkScale = 3.f;
		const vec3 Up = { 0,-1,0 };
		for (int32_t i = 0; i < 25; ++i)
		{
			vec3 Dir = math::RotationVec(Up, D3DXToDegree( math::Rand<float>({ -math::PI / 2.f, math::PI / 2.f }) ) );
			float Rich = math::Rand<float>({ 40,270});
			vec3 EftLocation = Location + Dir * Rich;
			for (int32_t i = 0; i < 3; ++i)
			{
				std::wstring StateKey = L"Gunspark" + std::to_wstring(i);
				EffectManager::instance().EffectPush(L"Effect", StateKey, 8,
					0.1f, 8 * 0.1f + 0.01f, OBJECT_ID::EID::BOSS_RECOVER_BOMB, true, EftLocation,
					{ 0,0,0 }, { GunSparkScale,GunSparkScale,0 }, false, true, false, false, 25, 25,
					255, false, 0, atan2f(Dir.y, Dir.x), 0, 0, false, false, 0, false);

				std::wstring SmokeStateKey = L"spr_gunsmoke" + std::to_wstring(i);
				EffectManager::instance().EffectPush(L"GunSmoke", SmokeStateKey,
					10, 0.2f, 10 * 0.2f + 0.01f, OBJECT_ID::EID::ENONE, true,
					EftLocation, Dir * 100.f, { GunSparkScale ,GunSparkScale ,0 },
					false, false, false, false, 0, 0, 255, false, 0, atan2f(Dir.y, Dir.x), 0, 0, false);

			}
		}

		//
		_RenderComp->PositionCorrection -=vec3{ 0,10,0 };
		Recover();
	}
}

void Boss::HurtFly()
{
	_BossState = Boss::State::HurtFly;
	_PhysicComp->bFly = true;
	_PhysicComp->bLand = false;
	_RenderComp->Anim(false, true, L"spr_headhunter_hurtfly", 4, 0.5f, {});
	_PhysicComp->bGravity = true;
}

void Boss::HurtFlyState()
{
	if (_PhysicComp->bLand)
	{
		Hurt();
	}
}

void Boss::Dice()
{
	int32_t _Dice;
	
	do {
		_Dice = math::Rand<int32_t>({ 0,3 });
	} while (_Dice == PrevDice);

	PrevDice = _Dice;
	switch (_Dice)
	{
	case 0:
		TakeOutRifle();
		break;
	case 1:
		PreDash();
		break;
	case 2:
		PreJump();
		break;
	case 3:
		TakeOutGun();
		break;
	default:
		break;
	}
}

void Boss::PutBackGun()
{
	_BossState = Boss::State::PutBackGun;
	RenderComponent::NotifyType _Notify;
	_Notify[7] = [this]() 
	{
		bPutBackGunEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_putbackgun", 7,
		0.5f, std::move(_Notify));
}

void Boss::PutBackGunState()
{
	if (bPutBackGunEnd)
	{
		bPutBackGunEnd = false;
		Dice();
	}
	
}

void Boss::PutBackRifle()
{
	_BossState = Boss::State::PutBackRifle;
	RenderComponent::NotifyType _Notify;
	_Notify[8] = [this]()
	{
		bPutBackRifleEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_putbackrifle", 8,
		0.6f, std::move(_Notify));
}

void Boss::PutBackRifleState()
{
	if (bPutBackRifleEnd)
	{
		bPutBackRifleEnd = false;

		// 상태전이..
		Dice();
	};
}

void Boss::RecoverState()
{
	if (bRecoverEnd)
	{
		bRecoverEnd = false;
		bool Rand =math::Rand<int32_t>({ 0,1 });

		_PhysicComp->Position.x = Rand ? 2663 : 1240;
		_PhysicComp->Position.y = 1726.f;
		Dice();
	}
}

void Boss::Recover()
{
	_BossState = Boss::State::Recover;
	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]() 
	{
		bRecoverEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_headhunter_recover",
		4, 0.5f, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255));
}




