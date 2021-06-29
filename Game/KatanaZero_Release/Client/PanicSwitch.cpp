#include "stdafx.h"
#include "PanicSwitch.h"

#include "Player.h"
#include "Time.h"
#include "ObjectManager.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "PhysicTransformComponent.h"
#include "EffectManager.h"
#include "LaserTrap.h"

OBJECT_ID::EID PanicSwitch::GetID()
{
    return OBJECT_ID::EID::PANIC_SWITCH;
}

OBJECT_TAG::ETAG PanicSwitch::GetTag()
{
    return OBJECT_TAG::ETAG::INTERACTION_OBJECT;
}

std::wstring_view PanicSwitch::GetName() const&
{
    return L"PanicSwitch"sv;
}

void PanicSwitch::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo);

    if (_CollisionInfo._ID == OBJECT_ID::EPLAYER)
    {
        bInteraction = true;
        _RenderCompPressKey->bRender = true;
          auto _SpTarget =  _Target.lock();
          auto _Player = std::dynamic_pointer_cast<Player>(_SpTarget); 
          _Player->_CurPanitSwitch = _This;
    }
}

void PanicSwitch::Update()
{
    _RenderCompPressKey->bRender = false;
    bInteraction = false;
}

void PanicSwitch::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert<PhysicTransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent >(_This);
    _RenderCompPressKey = ComponentManager::instance().Insert<RenderComponent >(_This);

    _RenderComp->SlowColor = D3DCOLOR_ARGB(150, 255, 0, 0);
    _RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 255);
    _RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 255, 0, 0);
    _RenderComp->bRender = true;
    _RenderComp->PositionCorrection = vec3{ 0,0,0 };
    _RenderComp->Anim(true, false, L"Switch", 2, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        1, { 1,1 }, L"Panic_Switch", LAYER::ELAYER::EOBJECT_UNDER);

    _RenderCompPressKey->bRender = false;
    _RenderCompPressKey->PositionCorrection = vec3{ 0,-50,0 };
    _RenderCompPressKey->Anim(false, true, L"spacebar", 2, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
    0, { 1,1 }, L"spr_spacebar", LAYER::ELAYER::EUI);

    _CollisionComp->_CollisionInfo._ShapeType =
        CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 50;
    _CollisionComp->_CollisionInfo.Width = 50;
    _CollisionComp->_Tag = CollisionComponent::ETag::EInteractionObject;
    _CollisionComp->bCollision = true;

    _TransformComp->bMapSlide = false;
    _TransformComp->Scale *= 2.5f;
    _TransformComp->bFollowOwner = false;


}

void PanicSwitch::Interaction()
{
    if (!bInteraction)return;

    if (bOn)
    {
        Off();
    }
    else
    {
        On();
    }
};

std::shared_ptr<class LaserTrap>  PanicSwitch::PushTrap(vec3 Location,const float YScale,
    vec3 Speed, float MovementDuration)
{
    auto _spTrap = ObjectManager::instance().InsertObject<LaserTrap>();
    _spTrap->Init(std::move(Location), YScale,
        std::move( Speed ) , MovementDuration);
    _Traps.push_back(_spTrap);
    return _spTrap;
}

void PanicSwitch::On()
{
    bOn = true;
    _RenderComp->Anim(true, false, L"Switch", 2, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        1);

    for (auto& wpTrap : _Traps)
    {
        auto spTrap = wpTrap.lock();
        if (!spTrap)continue;
        spTrap->On();
    }
}

void PanicSwitch::Off()
{
    bOn = false;
    _RenderComp->Anim(true, false, L"Switch", 1, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        0);

    for (auto& wpTrap : _Traps)
    {
        auto spTrap = wpTrap.lock();
        if (!spTrap)continue;
        spTrap->Off();
    }

}
