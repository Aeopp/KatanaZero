#pragma once
#include "object.h"
#include "TransformComponent.h"

class Camera :
    public object
{
public:
	virtual void Initialize()& noexcept override;
	virtual void Update()override;

	virtual OBJECT_ID::EID   GetID()override ;
	virtual OBJECT_TAG::ETAG GetTag()override;
	virtual std::wstring_view GetName()const& override;
private:
	vec3 CurrentCameraPos{ 0.f,0.f,0.f };
};

