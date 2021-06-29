#pragma once
#include "object.h"
#include "TransformComponent.h"

struct Shake
{
	float Force;
	vec3 Dir;
	float Duration;
};

class Camera :
    public object
{
public:
	virtual void Initialize()& noexcept override;
	virtual void Update()override;

	virtual OBJECT_ID::EID   GetID()override ;
	virtual OBJECT_TAG::ETAG GetTag()override;
	virtual std::wstring_view GetName()const& override;
	
	vec3 CameraLockLT{ 0,0,0 };
	vec3 CameraLockRB{ 1,1,1 };
	vec3 PrevCameraLocation{ 0,0,0 };

	bool bUpdate = true;
	bool bMouseFollow = false;
	void CameraShake(float force, vec3 dir, float duration);
private:
	vec3 CurrentCameraPos{ 0.f,0.f,0.f };
	bool Shaking(float Dt);
	vec3 _Shake{ 0,0,0 };
	std::vector<Shake> Shake_Vec{};
};

