#include "stdafx.h"
#include "LaserTrap.h"

#include "Character.h"
#include "Player.h"
#include "Time.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "PhysicTransformComponent.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "sound_mgr.h"


OBJECT_ID::EID LaserTrap::GetID()
{
    return OBJECT_ID::EID::LASER_TRAP;
}

OBJECT_TAG::ETAG LaserTrap::GetTag()
{
    return OBJECT_TAG::ETAG::INTERACTION_OBJECT;
}

std::wstring_view LaserTrap::GetName() const&
{
    return L"LaserTrap"sv;
}

void LaserTrap::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo);

    if (!bCollisionLaser && ( _CollisionInfo._ID == OBJECT_ID::EPLAYER
                             || _CollisionInfo._TAG == OBJECT_TAG::EENEMY) )
    {
        auto spTarget =_CollisionInfo._Target.lock();
        auto spCharacter = std::dynamic_pointer_cast<Character>(spTarget);
        if (!spCharacter->IsAlive())return;

        bCollisionLaser = true;
        _RenderComp->Anim(true, true, L"Collision", 2, 0.1f);

        ObjectManager::instance()._Camera.lock()->CameraShake(
            500.f, math::Rand<int32_t>({ 0,1 }) ? vec3{1, 0, 0} : vec3{-1, 0, 0}, 0.5f);

        Time::instance().TimerRegist(0.5f, 0.5f, 0.5f, [this,This=_This]() {
            if (This.expired()) return true;
            this->bCollisionLaser = false;
            _RenderComp->Anim(true, true, L"NoCollision", 4, 0.15f);
            return true; });
    };
}

void LaserTrap::Update()
{
    const float dt  =Time::instance().Delta();
    CurMovementDuration -= dt;
    if (CurMovementDuration < 0.f)
    {
        CurMovementDuration = MovementDuration;
        _TransformComp->Dir *= -1.f;
    }

    _TransformComp->Position += _TransformComp->Dir * dt;
}

void LaserTrap::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert<PhysicTransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent >(_This);
    _RenderCompCellingGun = ComponentManager::instance().Insert<RenderComponent >(_This);

    _RenderComp->SlowColor = D3DCOLOR_ARGB(150, 255, 0, 0);
    _RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 255);
    _RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 255, 0, 0);
    _RenderComp->bRender = true;
    _RenderComp->PositionCorrection = vec3{ 0,0,0 };
    _RenderComp->Anim(true, true, L"NoCollision", 4, 0.1f, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        0, { 1,1 }, L"Laser", LAYER::ELAYER::EOBJECT_UNDER);

    _RenderCompCellingGun->bRender = true;
    _RenderCompCellingGun->PositionCorrection = vec3{ 0,-115,0 };
    _RenderCompCellingGun->Anim(true, true, L"spr_ceiling_laser_tread_on", 3, 0.1f, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        0, { 1,1 }, L"Laser", LAYER::ELAYER::EOBJECT_OVER);
    _RenderCompCellingGun->bFixedScale = true;
    _RenderCompCellingGun->FixedScale = { 2.5,2.5,1 };

    _CollisionComp->_CollisionInfo._ShapeType =
        CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 220;
    _CollisionComp->_CollisionInfo.Width = 5;
    _CollisionComp->_Tag = CollisionComponent::ETag::ELASER_TRAP;
    _CollisionComp->bCollision = true;

    _TransformComp->bMapSlide = false;
    _TransformComp->Scale *= 1;
    _TransformComp->bFollowOwner = false;


    _RenderComp->bFixedScale = true;
    _RenderComp->FixedScale = { 5,1,1 };

    bOn = true;
}

void LaserTrap::Init(vec3 Location, float YScale,vec3 Speed, float MovementDuration)
{
    _TransformComp->Position = std::move(Location);
    _TransformComp->Scale.y = YScale;
    _RenderCompCellingGun->PositionCorrection.y *= YScale;
    _RenderComp->FixedScale.y = YScale;


    _TransformComp->Dir = Speed;
    CurMovementDuration = this->    MovementDuration = MovementDuration;
}

void LaserTrap::On()
{
    bOn = true;
    _CollisionComp->bCollision = true;
    _RenderComp->bRender = true;
    _RenderCompCellingGun->Anim(true, true, L"spr_ceiling_laser_tread_on", 3,
        0.1f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 });
    
    SOUNDPLAY("lasers_on", 1.f);

}

void LaserTrap::Off()
{
    bOn = false;
    _CollisionComp->bCollision = false;
    _RenderComp->bRender = false;
    _RenderCompCellingGun->Anim(true, true, L"spr_ceiling_laser_tread_off", 3,
        0.1f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 });

    SOUNDPLAY("lasers_off", 1.f);
}
