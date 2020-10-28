#include "stdafx.h"
#include "Enemy.h"
#include "ObjectManager.h"
#include "CollisionComponent.h"

OBJECT_TAG::ETAG Enemy::GetTag()
{
    return OBJECT_TAG::ETAG::ENEMY;
}

void Enemy::Initialize() & noexcept
{
    Super::Initialize();

    _Target = ObjectManager::instance()._Player.lock();
    _CollisionComp->_Tag = CollisionComponent::ETag::EEnemy;
};
