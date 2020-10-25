#include "stdafx.h"
#include "Player.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "InputManager.h"
#include "Time.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "CollisionComponent.h"
#include "Battery.h"
#include "UIItemIcon.h"
#include "PhysicTransformComponent.h"
#include "math.h"

OBJECT_ID::EID Player::GetID()
{
	return OBJECT_ID::EPLAYER;
}

OBJECT_TAG::ETAG Player::GetTag()
{
	return Super::GetTag();
}

std::wstring_view Player::GetName() const&
{
	return L"Player"sv;
}

void Player::Initialize() & noexcept
{
	Super::Initialize();

	_TransformComp=ComponentManager::instance().Insert< PhysicTransformComponent>(_This);

	_TransformComp->Scale *= 3.f;
	auto _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	_PhysicComp->Mass = 100.f;

	_RenderComp->Anim(false, true,
		L"spr_dragon_idle", 12, 12* AnimFrameSpeed,
		{}, D3DCOLOR_ARGB(255, 255, 255, 255),
		0.f, vec2{ 1.f,1.f }, L"Dragon",
		LAYER::ELAYER::EOBJECT);

	_CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 40;
	_CollisionComp->_CollisionInfo.Width = 20;

	KeyBinding();

	_SpCamera= ObjectManager::instance().InsertObject<Camera>();
	_SpCamera->SetOwner(_This);
	// TODO :: 아이템 획득시 배터리에게 통보해주기.
	_SpBattery = ObjectManager::instance().InsertObject<Battery>();
	_SpBattery->SetOwner(_This);

	_SpUIItemIcon = ObjectManager::instance().InsertObject<UIItemIcon>();
	_SpUIItemIcon->SetOwner(_This);


	_PhysicComp->bGravity = true;

	Speed = PlayerSpeed;

	TimeRegist();
}

void Player::LateInitialize() & noexcept
{
	Super::LateInitialize();
}

void Player::Release() & noexcept
{
	Super::Release();
}

void Player::Update()
{
	Super::Update();
}

void Player::LateUpdate()
{
	Super::LateUpdate();

	// fly 가 on 이고 wallride 가 false 이고 jump 애니메이션 재생이 끝났다면
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

	if (PhysicComp->bFly && !bWallRide && bJumpStartAnimEnd)
	{
		_RenderComp->Anim(false, true, L"spr_dragon_fall", 41
			, 0.2f);
	};

	if (PhysicComp->bLand && !PhysicComp->bFly  && 
			!bAttacking && !bCrouch && !CurrentFrameCharacterInput &&
		!bMovingOn)
	{
		_RenderComp->Anim(false, true, L"spr_dragon_idle", 12, 12 * AnimFrameSpeed);
	};

	CurrentFrameCharacterInput = false;
};

void Player::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	constexpr float WallJumpAngle = 30.f;
	const vec3 UpVec = { 0.f,-1.f,0.f };

	if (_CollisionInfo._ID == OBJECT_ID::EWALLRIDELINE)
	{
		WallRideDir = *_CollisionInfo._Variable._Cast<vec3>();
		D3DXVec3Lerp(&WallRideDir, &WallRideDir, &UpVec, 0.5f);
	}

	auto _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

	/*if (!_PhysicComp->bFly && _CollisionInfo._TAG ==OBJECT_TAG::ETERRAIN
		 && _CollisionInfo._ID !=OBJECT_ID::EWALLRIDELINE 
		&&_PhysicComp->bLand  && !bMovingOn && bJumpStartAnimEnd && !bWallRide 
		&&!bWallJump && !CurrentFrameCharacterInput)
	{
		_RenderComp->Anim(false, true, L"spr_dragon_idle", 12, 12 * AnimFrameSpeed);
	}*/
};

void Player::Move(vec3 Dir,const float AddSpeed)
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent>
		(_TransformComp);

	if (!bMovingOn && !_Physic_TransformComp->bFly)
	{
		MoveStart(Dir);
		return;
	}

	if (!bWallRide)
	{
		if (_Physic_TransformComp->bFly )
		{
			Speed = PlayerSpeed*0.5f ;
		}
		else if (bSneak)
		{
			Speed = PlayerSpeed * 0.6f; 
			_RenderComp->Anim(false, true, L"spr_dragon_sneak", 10,
				10 * AnimFrameSpeed);
		}
		else
		{
			_RenderComp->Anim(false, true, L"spr_dragon_run", 10, 10 * AnimFrameSpeed);
			Speed = PlayerSpeed;
		}

		Super::Move(Dir, AddSpeed);
	}
}

void Player::FSM()
{
	switch (_CurrentAnimState)
	{
	case Player::State::Attack:
		break;
	case Player::State::Crouch:
		break;
	case Player::State::Dash:
		break;
	case Player::State::DoorKick:
		break;
	case Player::State::Fall:
		break;
	case Player::State::Flip:
		break;
	case Player::State::HurtFly:
		break;
	case Player::State::HurtFly_Begin:
		break;
	case Player::State::Hurt_Ground:
		break;
	case Player::State::HurtRecover:
		break;
	case Player::State::Idle:
		
		break;
	case Player::State::Idle_to_Run:
		break;
	case Player::State::Idle_to_sneak:
		break;
	case Player::State::Jump:
		break;
	case Player::State::Post_Crouch:
		break;
	case Player::State::Pre_Crouch:
		break;
	case Player::State::Roll:
		break;
	case Player::State::Run:
		break;
	case Player::State::Run_to_Idle:
		break;
	case Player::State::Sneak:
		break;
	case Player::State::StairFall:
		break;
	case Player::State::Wall_Ride:
		break;
	default:
		break;
	}
}

void Player::MoveStart(const vec3 Dir)
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (PhysicComp->bFly)return;

	RenderComponent::NotifyType _Notify;

	_RollCheck[0] = true;
	RollDir = Dir;

	_Notify[3] = [this]()
	{
		bMovingOn = true;
	};

	if (!bSneak)
	{
		_RenderComp->Anim(false, false, L"spr_dragon_idle_to_run",
			3, 3 * AnimFrameSpeed, std::move(_Notify));
	}
	else
	{
		_RenderComp->Anim(false, false, L"spr_dragon_idle_to_sneak",
			4, 4 * AnimFrameSpeed);
	}
}
void Player::MoveEnd(const vec3 Dir)
{
	if (bMovingOn)
	{
		Time::instance().TimerRegist(4 * AnimFrameSpeed, 4 * AnimFrameSpeed, 4 * AnimFrameSpeed,
			[this, Observer = _This]()
		{
			if (!Observer.expired())
			{
				bMovingOn = false;
			}
			return true;
		});

		_RollCheck[0] = false;

		if (!bSneak)
		{
			_RenderComp->Anim(false, false, L"spr_dragon_run_to_idle",
				4, 4 * AnimFrameSpeed );
		}
	}
};

void Player::KeyBinding() & noexcept
{
	auto Observer = _This;

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{		
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		const vec3 Dir{ 1.f,0.f,0.f };
		Move(Dir,0.f);
	},
		'D', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{ 	
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		const vec3 Dir{ -1.f,0.f,0.f };
		Move(Dir, 0.f); 
	},	'A', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		const vec3 Dir{ 1.f,0.f,0.f };
		MoveEnd(Dir);
	},
		'D', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		const vec3 Dir{ -1.f,0.f,0.f };
		MoveEnd(Dir);
	}, 'A', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		bSneak = true;
	}, VK_CONTROL, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		bSneak = false;
	}, VK_CONTROL, InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,  Observer]()
	{		
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		DownJump();
		Roll();
		PreCrouch();
	}, 'S', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		Crouch();
	}, 'S', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		PostCrouch();
	}, 'S', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,  Observer]()
	{ 	
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		if (bWallRide)
			JumpWallRide();
		else 
			Jump();
	},'W', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		Attack();

	}, VK_LBUTTON, InputManager::EKEY_STATE::DOWN));
	
	_Anys.emplace_back(InputManager::instance().EventRegist([this,Observer]()
	{
		if (!object::IsValid(Observer))return;
		Time::instance().TimeScale = 0.1f;
	},
		VK_SHIFT, InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this ,Observer]()
	{
		if (!object::IsValid(Observer))return;
		Time::instance().TimeScale = 1.f;
	},
		VK_SHIFT, InputManager::EKEY_STATE::UP));

	//////////
}

void Player::Jump()
{
	auto PhysicComp =std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;
	if (CurrentJumpCoolTime > 0)return;

	CurrentJumpCoolTime = JumpCoolTime;

	PhysicComp->Flying();

	const vec3 Dir{ 0.f,-1.f,0.f };
	SimplePhysics _Physics;
	_Physics.Acceleration = 100.f;
	_Physics.Dir = Dir;
	_Physics.Speed = { 0.f,-800.f,0.f };
	_Physics.MaxT = JumpCoolTime;

	PhysicComp->Move(std::move(_Physics));

	bJumpStartAnimEnd = false;

	RenderComponent::NotifyType _Nofity;
	_Nofity[4] = [this]() 
	{
		bJumpStartAnimEnd = true;
	};

	_RenderComp->Anim(false, false, L"spr_dragon_jump", 4, 4 * AnimFrameSpeed,std::move(_Nofity));
}

void Player::DownJump()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

	if (!PhysicComp->bDownLand)
	{
		_RollCheck[1] = true;
		return;
	}
	else
	{
		_RollCheck[1] = false;
	}

	PhysicComp->Flying();
	_CollisionComp->bDownJump = true;

	Time::instance().TimerRegist(0.5f, 0.f, 0.5f,
		[wpThis = _This, this]()
	{
		if (!object::IsValid(wpThis))return true;
		_CollisionComp->bDownJump = false;
		return false;
	});

	const vec3 Dir{ 0.f,-1.f,0.f };
	SimplePhysics _Physics;
	_Physics.Acceleration = 100.f;
	_Physics.Dir = Dir;
	_Physics.Speed = { 0.f,+400.f,0.f };
	_Physics.MaxT = 0.3f;

	PhysicComp->Move(std::move(_Physics));
}

void Player::Attack()
{
	if (bAttacking) return ;
	
	RenderComponent::NotifyType _Notify;

	bAttacking = true;

	_Notify[2] = [this]() { AttackSlash (); };
	_Notify[7] = [this]() { bAttacking = false;  };

	_RenderComp->Anim(false, false,
		L"spr_dragon_attack",
		7,
		7 * AnimFrameSpeed, std::move(_Notify));

	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;

	_Physic_TransformComp->Position.z = 0.f;

	vec3 Distance = global::MousePosWorld - _Physic_TransformComp->Position;
	vec3 Dir;
	D3DXVec3Normalize(&Dir, &Distance);
	SimplePhysics _Physics;

	_Physics.Acceleration = AttackAcceleration;
	_Physics.Dir = Dir;
	_Physics.Speed = Dir * AttackForce;
	_Physics.MaxT = 7 * AnimFrameSpeed;
	_Physics.T = 0.f;

	_Physic_TransformComp->Move(std::move(_Physics));
}

void Player::AttackSlash()
{
	// TODO :: 여기서 슬래쉬 객체에게 공격 요청
}

void Player::Roll()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (PhysicComp->bDownLand)return;
	if (!PhysicComp->bLand)return;
	if (PhysicComp->bFly)return;
	if (!_RollCheck.all())return;
	if (CurrentRollCoolTime > 0.f)return;

	CurrentRollCoolTime = RollCoolTime;

	SimplePhysics _Physics;
	_Physics.Acceleration = 200.f;
	_Physics.Dir = RollDir;
	_Physics.Speed = RollDir * RollSpeed;
	_Physics.MaxT = RollCoolTime;
	_Physics.T = 0.0f;

	PhysicComp->Move(std::move(_Physics));

	_RenderComp->Anim(false, false,
		L"spr_dragon_roll", 7, 7 * AnimFrameSpeed);
}

void Player::TimeRegist()
{
	auto Obsever = _This;

	Time::instance().TimerRegist(0.f, 0.f, FLT_MAX,[this,Obsever]() 
	{
		if (Obsever.expired())return true;
		const float Dt =Time::instance().Delta();
		CurrentJumpCoolTime -= Dt;
		CurrentWallJumpCoolTime -= Dt;
		CurrentRollCoolTime -= Dt;
		return false;
	});
}

void Player::PreCrouch()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;
	if (PhysicComp->bFly)return;
	if (bWallRide)return;

	bCrouch = true;

	_RenderComp->Anim(false, false, L"spr_dragon_precrouch",
		2, 2 * AnimFrameSpeed);
}

void Player::Crouch()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;
	if (PhysicComp->bFly)return;
	if (bWallRide)return;
	if (!bCrouch)return;

	_RenderComp->Anim(false, true, L"spr_dragon_crouch", 1, 1 * AnimFrameSpeed);
}

void Player::PostCrouch()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;
	if (PhysicComp->bFly)return;
	if (bWallRide)return;
	if (!bCrouch)return;

	RenderComponent::NotifyType _Notify;
	_Notify[2] = [this]()
	{
		bCrouch = false;
	};

	_RenderComp->Anim(false, false, L"spr_dragon_postcrouch",
		2, 2 * AnimFrameSpeed, std::move(_Notify));
}

void Player::WallRideEnd()
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;

	_Physic_TransformComp->GravityCoefficient = 1.f;
	bWallRide = false;
}

void Player::WallRide()
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;
	// 점프중이 아니라면 벽타기 불가능
	if (!_Physic_TransformComp->bFly)return;
	if (_Physic_TransformComp->bLand)return;

	// 벽에 매달리는 중이라면 가속도가 감소
	_Physic_TransformComp->GravityCoefficient = 0.2f;
	_Physic_TransformComp->Flying();

	_RenderComp->Anim(false, false, L"spr_dragon_wallride", 2, 2 * AnimFrameSpeed);
	bWallRide = true;
}

void Player::JumpWallRide()
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;
	if (CurrentWallJumpCoolTime > 0)return;

	CurrentWallJumpCoolTime = WallJumpCoolTime;
	_Physic_TransformComp->Flying();
	bWallRide = false;
	bWallJump = true;

	SimplePhysics _Physics;
	_Physics.Acceleration = 200.f;
	_Physics.Dir = WallRideDir;
	_Physics.Speed = WallRideDir *2000.f;
	_Physics.MaxT = WallJumpCoolTime;
	_Physics.T = 0.f;

	_Physic_TransformComp->Move(std::move(_Physics));

	_RenderComp->Anim(false, false, L"spr_dragon_flip",
		10,
		10 * AnimFrameSpeed);
}
