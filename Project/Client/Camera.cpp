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

	auto spObj = _Owner.lock();
	if (!spObj)return;

	const vec3 OwnerPosition = spObj->_TransformComp->Position; 

	CurrentCameraPos = OwnerPosition;
	CurrentCameraPos.x -= global::ClientSize.first / 2.f;
	CurrentCameraPos.y -= global::ClientSize.second / 2.f;

	if (global::_CurGameState != global::ECurGameState::Replay &&
		global::_CurGameState != global::ECurGameState::ReWind && bMouseFollow)
	{
		vec3 Goal = global::MousePosScreen;
		Goal.x -= global::ClientSize.first / 2.f;
		Goal.y -= global::ClientSize.second / 2.f;
		Goal = CurrentCameraPos + Goal * 0.8f;
		Shaking(Dt);
		Goal += _Shake;
		_Shake = { 0,0 ,0 };
		global::CameraPos = math::lerp(global::CameraPos, Goal, 0.5f, Time::instance().Delta());
		return;
	}
	else
	{
		Shaking(Dt);
		vec3 Goal = CurrentCameraPos;
		Goal += _Shake;
		_Shake = { 0,0 ,0 };
		global::CameraPos = math::lerp(global::CameraPos, Goal, 0.5f, Time::instance().Delta());
		
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
