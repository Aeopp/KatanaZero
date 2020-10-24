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
    return L"Character";
}

void Character::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert< PhysicTransformComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
};

void Character::Move(vec3 Dir, const float AddSpeed)
{
    const float DeltaTime = Time::instance().Delta();
    float _MoveSpeed = Speed;

    float AddY = 0.f;
    // ���п� �� �浹���̶�� ������ ���⸸ŭ Y��ǥ�� �����Ѵ�.
    if (_TransformComp->bLineMode)
    {
        float m = _TransformComp->CurrentLineDir.y / _TransformComp->CurrentLineDir.x;
        AddY = Dir.x * _MoveSpeed *  m;
    }

    const vec3 CurrentPos = _TransformComp->Position;

    vec3 GoalPos = _TransformComp->Position + (Dir * _MoveSpeed);
    GoalPos.y += AddY;
    _TransformComp->Position = math::lerp(CurrentPos, GoalPos, MoveGoalTime, DeltaTime);

    
}
