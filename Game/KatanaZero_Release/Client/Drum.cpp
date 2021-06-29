#include "stdafx.h"
#include "Drum.h"

#include "Time.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "PhysicTransformComponent.h"
#include "EffectManager.h"
#include "sound_mgr.h"

OBJECT_ID::EID Drum::GetID()
{
    return OBJECT_ID::EID::DRUM;
}

OBJECT_TAG::ETAG Drum::GetTag()
{
    return OBJECT_TAG::ETAG::INTERACTION_OBJECT;
}

std::wstring_view Drum::GetName() const&
{
    return L"Drum"sv;
}

void Drum::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo);

    if (_CollisionInfo._ID == OBJECT_ID::EID::EXPLOSION)
    {
        constexpr float ExplosiveRange = 50;
        vec3 Location = _TransformComp->Position;
        float ExplosiveRepeatTime = 0.05f;

        RAND_SOUNDPLAY("explosion", { 1,3 }, 1.f);

        Time::instance().TimerRegist(0.0f, 0.1f, 0.5f, [Location]() {
          
            vec3 InitLoc = Location + math::RandVec({ -1,1 })* math::Rand<float>({ 40,70 });
            EffectManager::instance().EffectPush(L"Effect", L"spr_explosion_1",
                11, 0.05f, 11 * 0.05f + 0.01f, OBJECT_ID::EID::EXPLOSION, true,
                InitLoc, { 0,0,0 },
                { 3,3,0 }, false, true, false, true, 33, 33,
                255, false, 0.f, 0.f, 0.f, 0);
            return false; });

        _CollisionComp->bCollision = false;
        _RenderComp->bRender = false;
    }
}

void Drum::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert<PhysicTransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent >(_This);
    
    _RenderComp->SlowColor = D3DCOLOR_ARGB(150, 255, 0, 0);
    _RenderComp->SlowGoalColor= D3DCOLOR_ARGB(0, 0, 0, 255);
    _RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 255, 0, 0);
    _RenderComp->bRender = true;
    _RenderComp->PositionCorrection = vec3{ 0,0,0 };
    _RenderComp->Anim(false, false, L"drum", 1, FLT_MAX, {},D3DCOLOR_ARGB(255,255,255,255),
        0, { 1,1 }, L"oil_drum", LAYER::ELAYER::EOBJECT);

    _CollisionComp->_CollisionInfo._ShapeType =
        CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 21;
    _CollisionComp->_CollisionInfo.Width = 27;
    _CollisionComp->_Tag = CollisionComponent::ETag::EInteractionObject;
    _CollisionComp->bCollision = true;

    _TransformComp->Scale *= 2.5f;
    _TransformComp->bMapSlide = true;
    _TransformComp->bFollowOwner = false;
}
