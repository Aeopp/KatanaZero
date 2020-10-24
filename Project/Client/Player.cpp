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

	_RenderComp->_RenderInfo.Number = 0;
	_RenderComp->_RenderInfo.ObjectKey = L"Dragon";
	_RenderComp->_RenderInfo.StateKey = L"spr_dragon_attack";
	_RenderComp->_RenderInfo._Layer = LAYER::EOBJECT;

	_CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 20;
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

	Speed = 1000.f;
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
}

void Player::Move( vec3 Dir,const float AddSpeed)
{
	Super::Move(Dir, AddSpeed);
}

void Player::KeyBinding() & noexcept
{
	auto wpThis = _This;

	_Anys.emplace_back(InputManager::instance().EventRegist([this, wpThis]()
	{		
		if (!object::IsValid(wpThis))return;
		const vec3 Dir{ 1.f,0.f,0.f };
		Move(Dir,0.f);
	},
		'D', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, wpThis]()
	{ 	
		if (!object::IsValid(wpThis))return;
		const vec3 Dir{ -1.f,0.f,0.f };
		Move(Dir, 0.f); 
	},	'A', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,  wpThis]()
	{		
		if (!object::IsValid(wpThis))return;
		DownJump();
	}, 'S', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,  wpThis]()
	{ 		if (!object::IsValid(wpThis))return;

		Jump();
	},'W', InputManager::EKEY_STATE::DOWN));

	//TODO:: TEST CODE
	_Anys.emplace_back(InputManager::instance().EventRegist([this, wpThis]()
	{ 
		if (!object::IsValid(wpThis))return;
		_RenderComp->_RenderInfo.StateKey = L"spr_dragon_idle";
	},
		'2', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,wpThis]()
	{
		if (!object::IsValid(wpThis))return;
		_RenderComp->_RenderInfo.StateKey = L"spr_dragon_attack";
	},
		'1', InputManager::EKEY_STATE::DOWN));
	
	_Anys.emplace_back(InputManager::instance().EventRegist([this,wpThis]()
	{
		if (!object::IsValid(wpThis))return;
		Time::instance().TimeScale = 0.1f;
	},
		VK_SHIFT, InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this ,wpThis]()
	{
		if (!object::IsValid(wpThis))return;
		Time::instance().TimeScale = 1.f;
	},
		VK_SHIFT, InputManager::EKEY_STATE::UP));
	//////////
}

void Player::Jump()
{
	auto PhysicComp =std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bLand)return;

	PhysicComp->Flying();

	const vec3 Dir{ 0.f,+1.f,0.f };
	SimplePhysics _Physics;
	_Physics.Acceleration = 500.f;
	_Physics.Dir = Dir;
	_Physics.Friction = 0.97f;
	_Physics.Speed = { 0.f,-1300.f,0.f };
	_Physics.Resistance = 0.4f;
	_Physics.MaxT = 9999999999.f;
	_Physics.T = 0.f;

	PhysicComp->Move(std::move(_Physics));
}

void Player::DownJump()
{
	auto PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
	if (!PhysicComp->bDownLand)return;

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
	_Physics.Acceleration = 500.f;
	_Physics.Dir = Dir;
	_Physics.Friction = 0.97f;
	_Physics.Speed = { 0.f,+2000.f,0.f };
	_Physics.Resistance = 0.4f;
	_Physics.MaxT = 9999999999.f;
	_Physics.T = 0.f;

	PhysicComp->Move(std::move(_Physics));
}
