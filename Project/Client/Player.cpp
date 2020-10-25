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

	FSM();
}

void Player::LateUpdate()
{
	Super::LateUpdate();

	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

	CurrentFrameCharacterInput = false;
	bMoveCurrentFrameKeyCheck = false;
	bJumpKeyCheck = false;
};

void Player::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CollisionInfo._ID == OBJECT_ID::EWALLRIDELINE)
	{
		const vec3 UpVec = { 0.f,-1.f,0.f };
		WallRideDir = *_CollisionInfo._Variable._Cast<vec3>();
		D3DXVec3Lerp(&WallRideDir, &WallRideDir, &UpVec, 0.5f);

		if (_CurrentState==Player::State::Fall)
		{
//			_PhysicComp->Flying();
			bWallRide = true;
		}
	}
};

void Player::Move(vec3 Dir,const float AddSpeed)
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent>
		(_TransformComp);

	CurrentMoveDir = Dir;
	bMoveCurrentFrameKeyCheck = true;

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
		}
		else
		{
			Speed = PlayerSpeed;
		}

		Super::Move(Dir, AddSpeed);
	}
}

void Player::Idle()
{
	// State 행동
	_RenderComp->Anim(false, true, L"spr_dragon_idle", 12, 1.f);
	////////////////////

	// 상태전이 검사.
	if (bMoveCurrentFrameKeyCheck)
		_CurrentState = Player::State::Idle_to_Run;
	if (bJumpKeyCheck)
		_CurrentState = Player::State::Jump;
	/// ///////////////////////////
}

void Player::IdleToRun()
{
	// State
	RenderComponent::NotifyType _Notify;
	_Notify[3] = [this]()
	{bIdleToRunMotionEnd = true;};
	_RenderComp->Anim(false, false, L"spr_dragon_idle_to_run", 3, 0.3f, std::move(_Notify));
	// ///////////////////////////////////////////////////

	// 상태전이
	if (!bMoveCurrentFrameKeyCheck)
		_CurrentState = Player::State::Idle;
	else if (bIdleToRunMotionEnd)
	{
		bIdleToRunMotionEnd = false;
		_CurrentState = Player::State::Run;
	}
	if (bJumpKeyCheck)
		_CurrentState = Player::State::Jump;
	////////////////////////////////////////////
}

void Player::Run()
{
	// State
	_RenderComp->Anim(false, true, L"spr_dragon_run", 10, 0.6f);
	// //

	// 상태 전이
	if (!bMoveCurrentFrameKeyCheck)
		_CurrentState = Player::State::Run_to_Idle;
	if (bJumpKeyCheck)
		_CurrentState = Player::State::Jump;
	//// 
}

void Player::RunToIdle()
{
	// State
	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]()
	{
		bRunToIdleMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_run_to_idle", 4,0.4f,std::move(_Notify));
	//  //

	// Translation 
	if (bRunToIdleMotionEnd)
	{
		bRunToIdleMotionEnd = false;
		_CurrentState = Player::State::Idle;
	}
	if (bJumpKeyCheck)
		_CurrentState = Player::State::Jump;
	// //
}

void Player::Fall()
{
	//State 
	_RenderComp->Anim(false, true, L"spr_dragon_fall", 4, 0.2f);
	_PhysicComp->GravityCoefficient = 0.3f;
	// // 

	// Transition 
	if (_PhysicComp->bLand)
		_CurrentState = Player::State::Idle;
	if (bWallRide)
		_CurrentState = Player::State::Wall_Ride;
	//
}

void Player::FSM()
{
	switch (_CurrentState)
	{
	case Player::State::Attack:
		Attack();
		break;
	case Player::State::Crouch:
		break;
	case Player::State::Dash:
		break;
	case Player::State::DoorKick:
		break;
	case Player::State::Fall:
		Fall();
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
		Idle();
		break;
	case Player::State::Idle_to_Run:
		IdleToRun();
		break;
	case Player::State::Idle_to_sneak:
		break;
	case Player::State::Jump:
		Jump();
		break;
	case Player::State::Post_Crouch:
		break;
	case Player::State::Pre_Crouch:
		break;
	case Player::State::Roll:
		break;
	case Player::State::Run:
		Run();
		break;
	case Player::State::Run_to_Idle:
		RunToIdle();
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
		bJumpKeyCheck = true;
	},'W', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		CurrentFrameCharacterInput = true;
		_CurrentState = Player::State::Attack;
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
	/*if (!_PhysicComp->bLand)return;
	if (CurrentJumpCoolTime > 0)return;*/

	/*CurrentJumpCoolTime = JumpCoolTime;*/

	//State 
	_PhysicComp->Flying();
	const vec3 Dir{ 0.f,-1.f,0.f };
	SimplePhysics _Physics;
	_Physics.Acceleration = 100.f;
	_Physics.Dir = Dir;
	_Physics.Speed = { 0.f,-800.f,0.f };
	_Physics.MaxT = JumpCoolTime;
	_PhysicComp->Move(std::move(_Physics));
	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]() 
	{
		bJumpAnimEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_jump", 4, 0.2f,std::move(_Notify));
	// // 

	//Transition
	if (bWallRide)
		_CurrentState = Player::State::Wall_Ride;
	if (bJumpAnimEnd)
		_CurrentState = Player::State::Fall;
	// //


	/*bJumpStartAnimEnd = false;*/
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
};

void Player::Attack()
{
	// State 

	// 애니메이션 
	RenderComponent::NotifyType _Notify;
	_Notify[7] = [this]() {bAttackMotionEnd = true; };
	_RenderComp->Anim(false, false, L"spr_dragon_attack", 7, 0.23f,std::move(_Notify));
	// // 

	// 움직임
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
	// // 

	// 전이
	if (bAttackMotionEnd)
	{
		bAttackMotionEnd = false;
		_CurrentState = Player::State::Fall;
	}
	// // 

	////////////////////////////////////////////
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
}

void Player::Crouch()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;
	if (PhysicComp->bFly)return;
	if (bWallRide)return;
	if (!bCrouch)return;
}

void Player::PostCrouch()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;
	if (PhysicComp->bFly)return;
	if (bWallRide)return;
	if (!bCrouch)return;
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
	// 벽에 매달리는 중이라면 가속도가 감소
	_PhysicComp->GravityCoefficient = 0.3f;
	_PhysicComp->Flying();
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
}
