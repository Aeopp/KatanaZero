#pragma once
#include "object.h"
class AttackObject :public object
{
public :
    using Super = object;
public :
    virtual OBJECT_ID::EID   GetID()            override ;
    virtual OBJECT_TAG::ETAG GetTag()           override      ;
    virtual std::wstring_view GetName()const&   override  ;
public :
    virtual void Initialize() & noexcept override;
    virtual void Update()override;
    virtual void Move(vec3 Dir, const float AddSpeed);

    std::shared_ptr<class PhysicTransformComponent> _PhysicComp;
    std::shared_ptr<class RenderComponent> _RenderComp;
    std::shared_ptr<class CollisionComponent> _CollisionComp;
public:
    float Speed = { 100.f };
    float MoveGoalTime = 1.f;
};

