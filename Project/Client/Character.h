#pragma once
#include "object.h"

class Character :
    public object
{
public :
	using Super = object;
public :
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
public :
	virtual void Initialize() & noexcept override;
	virtual void Update()override;
	virtual void LateUpdate()override;
	virtual void Move( vec3 Dir, const float AddSpeed);
	virtual bool IsAlive()const& abstract;
	std::shared_ptr<class PhysicTransformComponent> _PhysicComp;
	std::shared_ptr<class RenderComponent> _RenderComp;
	std::shared_ptr<class CollisionComponent> _CollisionComp;
protected:
	float Speed = 100.f;
	float MoveGoalTime = 2.f;

	bool bBloodingOverHead = false;
	bool bBlooding = false;
	float BloodingDelta = 0.1f;
	float BloodingOverHeadDelta = 0.1f;
	// 피격 마무리
	void BloodingOverHead();
	// 피격중 물리 받는 상태에서 호출
	void BloodingPastDir();
	// 피격당한 직후 호출 
	void BloodInit(vec3 Dir);
};

