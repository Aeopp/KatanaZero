#include "stdafx.h"
#include "Character.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "ObjectManager.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"

OBJECT_ID::EID Character::GetID()
{
    return OBJECT_ID::CHARACTER;
}

OBJECT_TAG::ETAG Character::GetTag()
{
    return OBJECT_TAG::CHARCTER;
}

std::wstring_view Character::GetName() const&
{ 
    return L"Character"sv;
}

void Character::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert< PhysicTransformComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);
};

void Character::Update()
{
    Super::Update();

    if (_TransformComp->Dir.x < 0.f)
    {
        _RenderComp->AnimDir = -1.f;
    }
    else
    {
        _RenderComp->AnimDir= 1.f;
    }
};

void Character::Move(vec3 Dir, const float AddSpeed)
{
    const float DeltaTime = Time::instance().Delta();

    float MoveFactor = 1.f;
    float AddY = 0.f;
    // 선분에 과 충돌중이라면 선분의 기울기만큼 Y좌표를 통제한다.
    if (_TransformComp->bLineMode)
    {
        MoveFactor = 0.6f;
        float m = _TransformComp->CurrentLineDir.y / _TransformComp->CurrentLineDir.x;
        AddY = Dir.x * ((Speed+ AddSpeed) * MoveFactor) *  m;
    }
    vec3 GoalPos = _TransformComp->Position + (Dir *  (( Speed + AddSpeed)* MoveFactor ) );
    GoalPos.y += AddY;
    _TransformComp->Position = math::lerp(_TransformComp->Position, GoalPos, MoveGoalTime, DeltaTime);
}
