#pragma once
#include "object.h"

class SmokeCloud :
    public object
{
public:
	enum class EState : uint8_t
	{
		Dissolve,
		Appear,
		Loop,
	};
    using Super = object;
	virtual OBJECT_ID::EID   GetID()override;
	virtual OBJECT_TAG::ETAG GetTag()override;
	virtual std::wstring_view GetName()const& override;

	void Initialize() & noexcept override;
	void Update()override;
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo);
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo);
	std::shared_ptr<class PhysicTransformComponent>_PhysicComp;
	void Start();
	int32_t Alpha = 255;
private:
	bool bAppear = false;
	float LoopDuration = 30.f;
	bool bHit = false;
	float TileHitTime = 0.0f;

	bool bEnd = true;

	std::shared_ptr<class CollisionComponent> _CollisionComp;
	std::shared_ptr<class RenderComponent> _RenderComp;
	
	SmokeCloud::EState _CurState = EState::Appear;
};

