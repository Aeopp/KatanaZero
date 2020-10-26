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

using namespace std;

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
	_PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	_PhysicComp->Mass = 100.f;

	_RenderComp->Anim(false, true,
		L"spr_dragon_idle", 12, 1.f,
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

	Speed= PlayerSpeed;

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

	FSM();
}

void Player::LateUpdate()
{
	Super::LateUpdate();

	const float Dt = Time::instance().Delta();

	bFrameCurrentCharacterInput = false;
	bMoveKeyCheck = false;
	bJumpKeyCheck = false; 
	bAttackKeyCheck = false;
	bDownKeyCheck = false;
	bCurWallRideCollision = false;

	CurAttackCoolTime -= Dt;
};

void Player::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CollisionInfo._ID == OBJECT_ID::EWALLRIDELINE && _PhysicComp->bFly)
	{
		const vec3 UpVec = { 0.f,-1.f,0.f };
		FlipDir = *_CollisionInfo._Variable._Cast<vec3>();
		D3DXVec3Lerp(&FlipDir, &FlipDir, &UpVec, 0.5f);
		bCurWallRideCollision = true;
		FSM();
	}
};

void Player::Move(vec3 Dir,const float AddSpeed)
{
	switch (_CurrentState)
	{
	case Player::State::Sneak:
		Speed = PlayerSpeed * 0.6f;
		break; 
	case Player::State::Fall:
		Speed = PlayerSpeed * 0.4f;
		break;
	case Player::State::Jump:
		Speed = PlayerSpeed * 0.4f;
		break;
	case Player::State::Run:
		Speed = PlayerSpeed;
		break;
	default:
		break;
	}

	Super::Move(Dir, AddSpeed);
}

void Player::Idle()
{
	// State 행동
	_CurrentState = Player::State::Idle;
	_RenderComp->Anim(false, true, L"spr_dragon_idle", 12, 1.f);
	////////////////////
}

void Player::IdleState()
{
	// 상태전이 검사.
	if (bMoveKeyCheck)
	{
		IdleToRun();
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
	}
	/// ///////////////////////////
}

void Player::IdleToRun()
{
	_CurrentState = Player::State::Idle_to_Run;

	RenderComponent::NotifyType _Notify;
	_Notify[3] = [this]()
	{bIdleToRunMotionEnd = true; };
	_RenderComp->Anim(false, false, L"spr_dragon_idle_to_run", 3, 0.12f , std::move(_Notify));
}

void Player::IdleToRunState()
{
	// 상태전이
	if (!bMoveKeyCheck)
	{
		Idle(); 
	}
	if (bIdleToRunMotionEnd)
	{
		Run();
		bIdleToRunMotionEnd = false;
	}
	if (bJumpKeyCheck)
	{
		Jump(); 
	}
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		Roll();
	}
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
	}
	////////////////////////////////////////////
}

void Player::Run()
{
	_CurrentState = Player::State::Run;

	// State
	_RenderComp->Anim(false, true, L"spr_dragon_run", 10, 0.6f);
	Move(_PhysicComp->Dir, 0.f);
	// //
}

void Player::RunState()
{
	Move(_PhysicComp->Dir, 0.f);

	if (!bMoveKeyCheck)
	{
		RunToIdle();
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}	
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		Roll();
	}
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
	}
}

void Player::RunToIdle()
{
	_CurrentState = Player::State::Run_to_Idle;

	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]()
	{
		bRunToIdleMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_run_to_idle", 4, 0.2f, std::move(_Notify));
}

void Player::RunToIdleState()
{
	if (bRunToIdleMotionEnd)
	{
		Idle(); 
		bRunToIdleMotionEnd = false;
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		Roll(); 
	}
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
	}
}

void Player::Fall()
{
	_CurrentState = Player::State::Fall;
	//State 
	_RenderComp->Anim(false, true, L"spr_dragon_fall", 4, 0.2f);
	// // 
}

void Player::FallState()
{
	if (_PhysicComp->bLand)
	{
		Idle(); 
	}	
	if (bCurWallRideCollision)
	{
		WallRide(); 
	}
	if (bMoveKeyCheck)
	{
		Move(_PhysicComp->Dir, 0.f);
	}
}

void Player::Jump()
{
	_CurrentState = Player::State::Jump;

	_PhysicComp->Flying();
	const vec3 Dir{ 0.f,-1.f,0.f };
	SimplePhysics _Physics;
	_Physics.Acceleration = 100.f;
	_Physics.Dir = Dir;
	_Physics.Speed = { 0.f,-800.f,0.f };
	_Physics.MaxT = 0.2f;
	_PhysicComp->Move(std::move(_Physics));

	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]()
	{
		bJumpAnimEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_jump", 4, 0.2f, std::move(_Notify));
}

void Player::FSM()
{
	switch (_CurrentState)
	{
	case Player::State::Attack:
		AttackState();
		break;
	case Player::State::Crouch:
		break;
	case Player::State::Dash:
		break;
	case Player::State::DoorKick:
		break;
	case Player::State::Fall:
		FallState();
		break;
	case Player::State::Flip:
		FlipState();
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
		IdleState();
		break;
	case Player::State::Idle_to_Run:
		IdleToRunState();
		break;
	case Player::State::Idle_to_sneak:
		break;
	case Player::State::Jump:
		JumpState();
		break;
	case Player::State::Post_Crouch:
		break;
	case Player::State::Pre_Crouch:
		break;
	case Player::State::Roll:
		RollState();
		break;
	case Player::State::Run:
		RunState();
		break;
	case Player::State::Run_to_Idle:
		RunToIdleState();
		break;
	case Player::State::Sneak:
		break;
	case Player::State::StairFall:
		break;
	case Player::State::Wall_Ride:
		WallRideState(); 
		break;
	case Player::State::DownJump : 
		DownJumpState();  
		break;
	default:
		break;
	}

	AnyState();
}

void Player::KeyBinding() & noexcept
{
	auto Observer = _This;

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{		
		if (!object::IsValid(Observer))return;
		bMoveKeyCheck = true;
		bFrameCurrentCharacterInput = true;
		const vec3 Dir{ 1.f,0.f,0.f };
		_PhysicComp->Dir = Dir;
	},
		'D', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{ 	
		if (!object::IsValid(Observer))return;
		bMoveKeyCheck = true;
		bFrameCurrentCharacterInput = true;
		const vec3 Dir{ -1.f,0.f,0.f };
		_PhysicComp->Dir = Dir;
	},	'A', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
	},
		'D', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
	}, 'A', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
	}, VK_CONTROL, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
	}, VK_CONTROL, InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
		bDownKeyCheck = true;
	}, 'S', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
	}, 'S', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,  Observer]()
	{ 	
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
		bJumpKeyCheck = true;
	},'W', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
		bAttackKeyCheck = true;
		_PhysicComp->Position.z = 0.f;
		vec3 MouseToDistance = global::MousePosWorld - _PhysicComp->Position;
		D3DXVec3Normalize(&_PhysicComp->Dir, &MouseToDistance);
		vec3 Dir = _PhysicComp->Dir;
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

void Player::JumpState()
{
	if (bCurWallRideCollision)
	{
		WallRide(); 
	}
	if (bJumpAnimEnd)
	{
		Fall();
		bJumpAnimEnd = false;
	}
	if (bMoveKeyCheck)
	{
		Move(_PhysicComp->Dir, 0.f);
	}
}

void Player::Attack()
{
	_CurrentState = Player::State::Attack;

	RenderComponent::NotifyType _Notify;
	_Notify[7] = [this]() {bAttackMotionEnd = true; };
	_RenderComp->Anim(false, false, L"spr_dragon_attack", 7, 0.23f, std::move(_Notify));
	
	_PhysicComp->Position.z = 0.f;
	vec3 Distance = global::MousePosWorld - _PhysicComp->Position;
	vec3 Dir;
	D3DXVec3Normalize(&Dir, &Distance);
	SimplePhysics _Physics;
	_Physics.Acceleration = 100.f;
	_Physics.Dir = Dir;
	_Physics.Speed = Dir * 300.f;
	_Physics.MaxT = 0.2f;
	_Physics.T = 0.f;
	_PhysicComp->Move(std::move(_Physics));
}

void Player::DownJump()
{
	_CurrentState = Player::State::DownJump;
	_PhysicComp->Flying();
	_CollisionComp->bDownJump = true;
	Time::instance().TimerRegist(0.2f, 0.2f, 0.2f,
		[wpThis = _This, this]()
	{
		if (!object::IsValid(wpThis))return true;
		_CollisionComp->bDownJump = false;
		return false;
	});

	_RenderComp->Anim(false, true, L"spr_dragon_fall", 4, 0.2f);
}
void Player::DownJumpState()
{
	if (!_CollisionComp->bDownJump)
	{
		Fall();
	}
}
void Player::DoorKick()
{
}
void Player::DoorKickState()
{
}
;

void Player::AttackState()
{
	if (bAttackMotionEnd)
	{
		bAttackMotionEnd = false;
		Fall();
	}
}

void Player::Roll()
{
	_CurrentState = Player::State::Roll;
	RenderComponent::NotifyType _Notify;
	_Notify[7] = [this]() 
	{
		bRollMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_roll",7, 0.4f ,std::move(_Notify));
	_PhysicComp->Move(_PhysicComp->Dir *1000.f, 100.f, 0.4f);
}

void Player::RollState()
{
	if (bJumpKeyCheck)
	{
		Jump();
	}
	if (bRollMotionEnd)
	{
		bRollMotionEnd = false;
		Idle();
	}
}

void Player::AttackSlash()
{
	// TODO :: 여기서 슬래쉬 객체에게 공격 요청
}

void Player::TimeRegist()
{
	auto Obsever = _This;

	Time::instance().TimerRegist(0.f, 0.f, FLT_MAX,[this,Obsever]() 
	{
		if (Obsever.expired())return true;
		const float Dt =Time::instance().Delta();
		return false;
	});
}

void Player::WallRide()
{
	_CurrentState = Player::State::Wall_Ride;
	_PhysicComp->Dir = FlipDir;
	_PhysicComp->Dir.x *= -1.f;
	_PhysicComp->GravityCoefficient = 0.3f;
	_RenderComp->Anim(false, false, L"spr_dragon_wallride", 2, 0.2f);
}
void Player::WallRideState()
{
	if (bJumpKeyCheck)
	{
		Flip();
		_PhysicComp->GravityCoefficient = 1.f;
	}
	if (_PhysicComp->bLand)
	{	
		_PhysicComp->GravityCoefficient = 1.f;
		Idle();
	}
}
void Player::Flip()
{
	_CurrentState = Player::State::Flip;

	RenderComponent::NotifyType _Notify;
	_Notify[10] = [this]() {
		bFlipMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_flip", 10, 0.35f , std::move(_Notify));
	_PhysicComp->Move(FlipDir * 1500.f, 100.f, 0.35f);
}
void Player::FlipState()
{
	if (bFlipMotionEnd)
	{
		bFlipMotionEnd = false;
		Fall();
	}
};

void Player::AnyState()
{
	if (bAttackKeyCheck && CurAttackCoolTime<0.f)
	{
		CurAttackCoolTime = 0.2f;
		Attack();
	}
}

void Player::JumpWallRide()
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;

	_Physic_TransformComp->Flying();
	bWallJump = true;

	SimplePhysics _Physics;
	_Physics.Acceleration = 200.f;
	_Physics.Dir = FlipDir;
	_Physics.Speed = FlipDir *2000.f;
	_Physics.MaxT = 0.25f;
	_Physics.T = 0.f;

	_Physic_TransformComp->Move(std::move(_Physics));
}
