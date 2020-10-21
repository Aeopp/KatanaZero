#include "stdafx.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "InputManager.h"
#include "Time.h"
#include "math.h"

void Camera::Initialize() & noexcept
{

};

void Camera::Update()
{
	// 여기서 널임
	auto spObj = _Owner.lock();
	if (!spObj)return;

	const vec3 OwnerPosition = spObj->_TransformComp->Position; 

	CurrentCameraPos = OwnerPosition;
	CurrentCameraPos.x -= global::ClientSize.first / 2.f;
	CurrentCameraPos.y -= global::ClientSize.second / 2.f;

	vec3 Goal = global::MousePosScreen;
	Goal.x -= global::ClientSize.first / 2.f;
	Goal.y -= global::ClientSize.second / 2.f;
	Goal = CurrentCameraPos + Goal * 0.5f;

	global::CameraPos = math::lerp(global::CameraPos, Goal, 0.5f, Time::instance().Delta());
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
