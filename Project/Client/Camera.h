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
	
	bool bMouseFollow = false;
	void CameraShake(float force, vec3 dir, float duration);
private:
	vec3 CurrentCameraPos{ 0.f,0.f,0.f };
	bool Shaking(float Dt);
	vec3 _Shake{ 0,0,0 };
	std::vector<Shake> Shake_Vec{};

};

