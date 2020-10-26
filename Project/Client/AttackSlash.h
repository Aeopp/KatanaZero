#pragma once
#include "AttackObject.h"

class Attack_Slash :public AttackObject
{
public :
    using Super = AttackObject;
public:
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
public:
	virtual void Initialize() & noexcept;
	virtual void LateInitialize() & noexcept;
	virtual void Release() & noexcept;
	virtual void Update();
	virtual void LateUpdate();
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo)override;
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
	virtual void Move(vec3 Dir, const float AddSpeed)override;

public:
	void AttackStart(vec3 AttackPos);
private:
	bool bSlashEffectEnd{ false };
};

