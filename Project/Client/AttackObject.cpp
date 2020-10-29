#include "stdafx.h"
#include "AttackObject.h"
#include "RenderComponent.h"
#include "ObjectManager.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"
#include "ComponentManager.h"


OBJECT_ID::EID AttackObject::GetID()
{
    return OBJECT_ID::EID::ATTACK;
}

OBJECT_TAG::ETAG AttackObject::GetTag()
{
    return OBJECT_TAG::ATTACK;
}

std::wstring_view AttackObject::GetName() const&
{
    return L"AttackObject"sv;
}

void AttackObject::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert< PhysicTransformComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

    _CollisionComp->_Tag = CollisionComponent::ETag::EPlayerAttack;
}

void AttackObject::Update()
{
    Super::Update();

    /*if (_TransformComp->Dir.x < 0.f)
    {
        _RenderComp->AnimDir = -1.f;
    }
    else
    {
        _RenderComp->AnimDir = 1.f;
    }*/
}

void AttackObject::Move(vec3 Dir, const float AddSpeed)
{
    const float DeltaTime = Time::instance().Delta();
    vec3 GoalPos = _TransformComp->Position + (Dir *( Speed + AddSpeed));
    _TransformComp->Position = math::lerp(_TransformComp->Position, GoalPos, MoveGoalTime, DeltaTime);
}
