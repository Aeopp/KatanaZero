#pragma once
#include "object.h"
#include "UIItemIcon.h"

class Item :
    public object
{
public :
	using Super = object;
	virtual OBJECT_ID::EID   GetID()override;
	virtual OBJECT_TAG::ETAG GetTag()override;
	virtual std::wstring_view GetName()const& override;

	void Initialize() & noexcept override;
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo);
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo);
	virtual void Update()override;
	EItem _ItemID = EItem::Knife;
	bool bThrow = false;
private:
	std::shared_ptr<class CollisionComponent> _CollisionComp;
	std::shared_ptr<class RenderComponent> _RenderComp;
	std::shared_ptr<class PhysicTransformComponent> _PhysicComp;

	float CurThrowRotZ = 0.f;
	float ThrowRotZSpeed = math::PI*3.f;
	vec3 ThrowDir{ 0,0,0 };

	bool bEq = false;
	bool bUse = false;

	vec3 EQEffectPosCorre = { 0,-100,0 };
	std::wstring EQStateKey = L"";
	
	static constexpr int32_t SmokeNum = 50;
	std::array<std::shared_ptr<class SmokeCloud>, SmokeNum > _Smoke;
	// spr_butcher_knife
	// spr_explosive_vial
	 // spr_smoke_vial
	void KnifeInteraction(math::Collision::HitInfo _Info);
	void ExplosiveInteraction(math::Collision::HitInfo _Info);
	void SmokeInteraction(math::Collision::HitInfo _Info);

	void BulletInteraction(const math::Collision::HitInfo& _Info);
public:
	void SetUpInitState(int32_t ItemID);
	void Throw(vec3 InitLocation,vec3 Dir );
	void Interaction(math::Collision::HitInfo _Info);
};

