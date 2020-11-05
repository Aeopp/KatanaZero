#include "stdafx.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "InputManager.h"
#include "Time.h"
#include "math.h"
#include "Player.h"
#include "ObjectManager.h"
#include "Player.h"
#include <algorithm>

void Camera::Initialize() & noexcept
{
	_Shake= vec3{ 0,0,0 };

	_Anys.emplace_back(InputManager::instance().EventRegist(
		[this]() 
	{
		bMouseFollow = true;
	}, VK_CONTROL, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist(
		[this]()
	{
		bMouseFollow = false;
	}, VK_CONTROL, InputManager::EKEY_STATE::UP));
};

void Camera::Update()
{
	// 여기서 널임
	const float Dt = Time::instance().Delta();
	const float T = Time::instance().T();
	if (!bUpdate)return;

	const vec3 OwnerPosition = ObjectManager::instance()._Player.lock()->_TransformComp->Position;

	CurrentCameraPos = OwnerPosition;
	CurrentCameraPos.x -= global::ClientSize.first / 2.f;
	CurrentCameraPos.y -= global::ClientSize.second / 2.f;

	if (bMouseFollow &&  global::IsPlay()  )
	{
		vec3 Goal = global::MousePosScreen;
		Goal.x -= global::ClientSize.first / 2.f;
		Goal.y -= global::ClientSize.second / 2.f;
		Goal = CurrentCameraPos + Goal * 0.8f;
		Shaking(Dt);
		Goal += _Shake;
		_Shake = { 0,0 ,0 };

		Goal.x = std::clamp<float>(Goal.x, CameraLockLT.x, CameraLockRB.x - global::ClientSize.first + global::ClientSize.first * 0.1);
		Goal.y = std::clamp<float>(Goal.y, CameraLockLT.y, CameraLockRB.y - global::ClientSize.second + global::ClientSize.second * 0.1);

	/*	global::CameraPos.x = std::clamp<float>(Goal.x, CameraLockLT.x, CameraLockRB.x - global::ClientSize.first + global::ClientSize.first * 0.1);
		global::CameraPos.y = std::clamp<float>(Goal.y, CameraLockLT.y, CameraLockRB.y - global::ClientSize.second + global::ClientSize.second * 0.1);*/

		global::CameraPos = math::lerp(global::CameraPos, Goal, 0.6f, Time::instance().Delta());

		return;
	}
	else if ( global::IsPlay()  && !bMouseFollow)
	{
		Shaking(Dt);
		vec3 Goal = CurrentCameraPos;
		Goal += _Shake;
		_Shake = { 0,0 ,0 };

		Goal.x = std::clamp<float>(Goal.x, CameraLockLT.x, CameraLockRB.x - global::ClientSize.first + global::ClientSize.first * 0.1);
		Goal.y = std::clamp<float>(Goal.y, CameraLockLT.y, CameraLockRB.y - global::ClientSize.second + global::ClientSize.second * 0.1);
	/*	global::CameraPos.x = std::clamp<float>(Goal.x, CameraLockLT.x, CameraLockRB.x - global::ClientSize.first + global::ClientSize.first * 0.1);
		global::CameraPos.y = std::clamp<float>(Goal.y, CameraLockLT.y, CameraLockRB.y - global::ClientSize.second + global::ClientSize.second * 0.1);*/

		global::CameraPos = math::lerp(global::CameraPos, Goal, 0.6f, Time::instance().Delta());
		return;
	}
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
}
bool Camera::Shaking(float Dt)
{
	std::erase_if(Shake_Vec, [Dt](auto& ShakeInfo) {
		ShakeInfo.Duration -= Dt;
		return ShakeInfo.Duration < 0; });

	for (auto& ShakeInfo : Shake_Vec)
	{
		float  rand = math::Rand<float>({ -ShakeInfo.Force,+ShakeInfo.Force});
		_Shake+= ShakeInfo.Dir* rand;
	}

	return true;
}
void Camera::CameraShake(float force, vec3 dir, float duration)
{
	Shake_Vec.push_back({ force, dir, duration });
}
