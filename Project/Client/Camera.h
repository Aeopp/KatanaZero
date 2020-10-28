#pragma once
#include "object.h"
#include "TransformComponent.h"

struct Shake
{
	float T = 0.f;
	float Coefficient = 1.f;
	float DeltaCoefficient = 1.f;
	vec3 Vec{ 1.f,0.f,0.f };
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

	void CameraShake(Shake _Shake);
private:
	vec3 CurrentCameraPos{ 0.f,0.f,0.f };
	std::vector<Shake> _ShakeVec;
};

