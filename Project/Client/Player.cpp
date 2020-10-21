#include "stdafx.h"
#include "Player.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "InputManager.h"
#include "Time.h"
#include "ObjectManager.h"
#include "Camera.h"

OBJECT_ID::EID Player::GetID()
{
	return OBJECT_ID::EPLAYER;
}

OBJECT_TAG::ETAG Player::GetTag()
{
	return OBJECT_TAG::EPLAYER;
}

std::wstring_view Player::GetName() const&
{
	return L"Player"sv;
}

void Player::Initialize() & noexcept
{
	object::Initialize();

	_TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
	_TransformComp->Scale *= 2.f;

	_RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);

	_RenderComp->_RenderInfo.Number = 0;
	_RenderComp->_RenderInfo.ObjectKey = L"Dragon";
	_RenderComp->_RenderInfo.StateKey = L"spr_dragon_attack";
	_RenderComp->_RenderInfo._Layer = LAYER::EOBJECT;

	auto wpThis = _This;

	_Anys.emplace_back(InputManager::instance().EventRegist([wpThis]()
	{ 
		const vec3 Dir{ 1.f,0.f,0.f };
		auto spObj =wpThis.lock(); 
		auto spPlayer =spObj->GetThis<Player>();
		spPlayer->Move(Dir);
	},
		VK_RIGHT, InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([wpThis]()
	{ const vec3 Dir{ -1.f,0.f,0.f };
	auto spObj = wpThis.lock();
	auto spPlayer = spObj->GetThis<Player>();
	spPlayer->Move(Dir);  },
		VK_LEFT, InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([wpThis]()
	{ const vec3 Dir{ 0.f,-1.f,0.f };
	auto spObj = wpThis.lock();
	auto spPlayer = spObj->GetThis<Player>();
	spPlayer->Move(Dir);  },
		VK_UP, InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([wpThis]()
	{ const vec3 Dir{ 0.f,+1.f,0.f };
	auto spObj = wpThis.lock();
	auto spPlayer = spObj->GetThis<Player>();
	spPlayer->Move(Dir);  },
		VK_DOWN, InputManager::EKEY_STATE::PRESSING));

	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->SetOwner(_This);
}

void Player::LateInitialize() & noexcept
{
	object::LateInitialize();
}

void Player::Release() & noexcept
{
	object::Release();
}

void Player::Update()
{
	object::Update();
}

void Player::LateUpdate()
{
	object::LateUpdate();
}

void Player::Move(const vec3 Dir)
{
	const float DeltaTime = Time::instance().Delta();

	_TransformComp->Position += Dir * Speed * DeltaTime;
}
