#include "stdafx.h"
#include "Character.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "ObjectManager.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"

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
}
