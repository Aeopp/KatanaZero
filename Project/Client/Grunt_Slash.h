#pragma once
#include "AttackObject.h"
class Grunt_Slash :
    public AttackObject
{
public :
    using Super = AttackObject;
public:
    virtual OBJECT_ID::EID   GetID();
    virtual OBJECT_TAG::ETAG GetTag();
    virtual std::wstring_view GetName()const&;
public:
	void Update()override;

	virtual void Initialize() & noexcept;
	virtual void LateUpdate();
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
public:
	void AttackStart(vec3 AttackPos, vec3 Dir);
private:
	bool bSlashEffectEnd{ false };
};

