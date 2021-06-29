#include "stdafx.h"
#include "Bullet.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"
OBJECT_ID::EID Bullet::GetID()
{
    return OBJECT_ID::EID::BULLET;
}

OBJECT_TAG::ETAG Bullet::GetTag()
{
    return OBJECT_TAG::ETAG::ENEMY_ATTACK;
}

std::wstring_view Bullet::GetName() const&
{
    return L"Bullet"sv;
}

void Bullet::Update()
{
    Super::Update();

    if (bBulletEffectEnd)
    {
        bBulletEffectEnd = false;
        _RenderComp->bRender = false;
        _CollisionComp->bCollision = false;
    }
}

void Bullet::Initialize() & noexcept
{
  //  Super::Initialize();
  //  Speed = 200.f;
  //  MoveGoalTime = 0.5f;

  //  _TransformComp->Scale *= 1.f;
  //  _PhysicComp->Mass = 100.f;

  //  RenderComponent::NotifyType _Notify;
  ///*  _Notify[5] = [this]()
  //  {
  //      bBulletEffectEnd= true;
  //  };*/
  //  _RenderComp->Anim()
  //  _RenderComp->Anim(false, true, L"spr_gruntslash", 5, 0.3f, move(_Notify),
  //      D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f },
  //      L"Grunt", LAYER::ELAYER::EEFFECT);
  //  _RenderComp->AfterImgOff();
  //  _RenderComp->bRender = false;

  //  _CollisionComp->bCollision = false;
  //  _CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
  //  _CollisionComp->_CollisionInfo.Height = 25;
  //  _CollisionComp->_CollisionInfo.Width = 25;
  //  _CollisionComp->PushForce = 200.f;
  //  _CollisionComp->_Tag = CollisionComponent::ETag::EEnemyAttack;

  //  _PhysicComp->bGravity = false;
  //  _PhysicComp->Position = vec3{ 0.f,0.f,0.f };
  //  _PhysicComp->Dir = { 1.f,0.f,0.f };

  //  Speed = 0.f;
  //  MoveGoalTime = 0.f;
}

void Bullet::LateUpdate()
{
}

void Bullet::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
}
