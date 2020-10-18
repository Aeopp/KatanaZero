#include "stdafx.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "InputManager.h"
#include "Time.h"

void Camera::Initialize() & noexcept
{
	_TransformComp=ComponentManager::instance().Insert<TransformComponent>(_This);

	InputManager::instance().EventRegist([this]() 
	{ /*if (!_TransformComp)return; */_TransformComp->Position += vec3{ 5.f,0.f,0.f }; MessageBox(global::hWND, L"호출", L"호출", MB_OK);  }, VK_UP, InputManager::EKEY_STATE::PRESSING)->bFree = true;

	InputManager::instance().EventRegist([this]()
	{/*if (!_TransformComp)return;*/ _TransformComp->Position += vec3{ -5.f,0.f,0.f }; }, VK_DOWN, InputManager::EKEY_STATE::PRESSING)->bFree=true ;

	//Time::instance().TimerRegist(5.f, 0.f, 5.f, [this]() {this->bDie = true; return true;  });

}

void Camera::Update()
{
	if (!_TransformComp)return;

	global::CameraPos = _TransformComp->Position;
}

OBJECT_ID::EID Camera::GetID()
{
	return OBJECT_ID::EID::CAMERA;
}

OBJECT_TAG::ETAG Camera::GetTag()
{
	return OBJECT_TAG::ETAG::EOBJECT;
}

std::wstring_view Camera::GetName() const&
{
	return L"Camera"sv;
}
