#pragma once
#include "object.h"
class Effect :
    public object
{
public :
	using Super = object;
public:
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
public :
	virtual void Initialize() & noexcept override;
	virtual void Update()override;
	virtual void Move(vec3 Dir, const float AddSpeed);
public :
	std::shared_ptr<class PhysicTransformComponent> _PhysicComp;
	std::shared_ptr<class RenderComponent> _RenderComp;
protected:
	float Speed = 100.f;
	float MoveGoalTime = 2.f;
};

