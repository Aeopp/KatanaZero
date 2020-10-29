#include "stdafx.h"
#include "Effect.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "ObjectManager.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"

OBJECT_ID::EID Effect::GetID()
{
    return OBJECT_ID::EID::Effect;
}

OBJECT_TAG::ETAG Effect::GetTag()
{
    return OBJECT_TAG::EEFFECT;
}

std::wstring_view Effect::GetName() const&
{
    return L"Effect"sv;
}

void Effect::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert< PhysicTransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
}

void Effect::Update()
{
    Super::Update();

    //if (_TransformComp->Dir.x < 0.f)
    //{
    //    _RenderComp->AnimDir = -1.f;
    //}
    //else
    //{
    //    _RenderComp->AnimDir = 1.f;
    //}
}

void Effect::Move(vec3 Dir, const float AddSpeed)
{
    const float DeltaTime = Time::instance().Delta();

    vec3 GoalPos = _TransformComp->Position + (Dir * (Speed + AddSpeed));

    _TransformComp->Position = math::lerp(_TransformComp->Position, GoalPos, MoveGoalTime, DeltaTime);
}
