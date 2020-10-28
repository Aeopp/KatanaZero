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
	const float Dt = Time::instance().Delta();

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

	for (auto _ShakeIter = begin(_ShakeVec); _ShakeIter !=end(_ShakeVec);)
	{
		auto& _Shake = *_ShakeIter;
		_Shake.T += Dt *_Shake.DeltaCoefficient;

		float factor = sinf(_Shake.T) * _Shake.Coefficient;
		global::CameraPos += /*_Shake.Vec*/math::RandVec({ -1.f,+1.f }) *factor;

		if (_Shake.T > 1.f)
		{
			_ShakeIter = _ShakeVec.erase(_ShakeIter);
		}
		else
			++_ShakeIter; 
	}

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
}
void Camera::CameraShake(Shake _Shake)
{
	_ShakeVec.push_back(std::move(_Shake));	
};

