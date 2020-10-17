#include "stdafx.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "InputManager.h"

void Camera::Initialize() & noexcept
{
	_TransformComp=	ComponentManager::instance().Insert<TransformComponent>(_This);

	InputManager::instance().EventRegist([this]() { if (!_TransformComp)return; _TransformComp->Position += vec3{ 5.f,0.f,0.f }; }, VK_RIGHT, InputManager::EKEY_STATE::PRESSING)->bFree = true;
	InputManager::instance().EventRegist([this]() {if (!_TransformComp)return; _TransformComp->Position += vec3{ -5.f,0.f,0.f }; }, VK_LEFT, InputManager::EKEY_STATE::PRESSING)->bFree=true ;
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
