#pragma once
#include "object.h"
class LaserTrap :
    public object
{
public:
    using Super = object;
    virtual OBJECT_ID::EID   GetID()override;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual std::wstring_view GetName()const& override;
    virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
    void Update()override;
    void Initialize() & noexcept override;

    void Init(vec3 Location, float YScale,
        vec3 Speed, float MovementDuration);
private:
    std::shared_ptr<class CollisionComponent> _CollisionComp;
    std::shared_ptr<class RenderComponent> _RenderComp;
    std::shared_ptr<class RenderComponent> _RenderCompCellingGun;

    float MovementDuration = 1.f;
    float CurMovementDuration = 1.f;
public :
    void On();
    void Off();
    bool bOn = true;
    bool bCollisionLaser = false;
};

