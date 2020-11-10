#pragma once
#include "object.h"
class PanicSwitch :
    public object
{
public :
    using Super = object;
    virtual OBJECT_ID::EID   GetID()override;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual std::wstring_view GetName()const& override;
    virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
    void Update()override;
    void Initialize() & noexcept override;
public:
    void Interaction();
    void PushTrap(vec3 Location, const float YScale,
        vec3 Speed,float MovementDuration);
private:
    std::shared_ptr<class CollisionComponent> _CollisionComp;
    std::shared_ptr<class RenderComponent> _RenderComp;
    std::shared_ptr<class RenderComponent> _RenderCompPressKey;

    std::vector<std::weak_ptr<class LaserTrap>> _Traps;
private:
    bool bInteraction = false; 
    void On();
    void Off();
    bool bOn = true;
};

