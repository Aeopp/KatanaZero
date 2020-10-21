#include "stdafx.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "InputManager.h"
#include "Time.h"

void Camera::Initialize() & noexcept
{

};

void Camera::Update()
{
	// 여기서 널임
	auto spObj = _Owner.lock();
	if (!spObj)return;

	global::CameraPos = spObj->_TransformComp->Position;
	global::CameraPos.x -= global::ClientSize.first / 2.f;
	global::CameraPos.y -= global::ClientSize.second / 2.f;

	int i = 0;
};

OBJECT_ID::EID Camera::GetID()
{
	return OBJECT_ID::EID::CAMERA;
};

OBJECT_TAG::ETAG Camera::GetTag()
{
	return OBJECT_TAG::ETAG::EOBJECT;
};

std::wstring_view Camera::GetName() const&
{
	return L"Camera"sv;
};
