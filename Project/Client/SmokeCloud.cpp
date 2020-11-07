#include "stdafx.h"
#include "SmokeCloud.h"

#include "Time.h"
#include "NormalEnemy.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "PhysicTransformComponent.h"
#include "Player.h"
#include "EffectManager.h"
#include "Effect.h"

OBJECT_ID::EID SmokeCloud::GetID()
{
    return OBJECT_ID::EID::SMOKE_CLOUD;
}

OBJECT_TAG::ETAG SmokeCloud::GetTag()
{
    return OBJECT_TAG::ETAG::SMOKE_CLOUD;
}

std::wstring_view SmokeCloud::GetName() const&
{
    return L"Smoke_Cloud"sv;
}

void SmokeCloud::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp = ComponentManager::instance().Insert<PhysicTransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent >(_This);
    _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

    _PhysicComp->Scale *= 3;
    _PhysicComp->Mass = 100.f;

    _RenderComp->AfterImgOff();
    _RenderComp->PositionCorrection = vec3{ 0,0,0 };
    /*_RenderComp->SlowDeltaCoefft = 0.05f;
    _RenderComp->NormalAfterImgPushDelta *= 1.0f;*/
    _RenderComp->bRender = false;

    _CollisionComp->_CollisionInfo._ShapeType =
        CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 28;
    _CollisionComp->_CollisionInfo.Width = 28;
    _CollisionComp->_Tag = CollisionComponent::ETag::ESmoke_Cloud;
    _CollisionComp->bCollision = false;
    _CollisionComp->bLineCollision = false;
    _CollisionComp->bTileCollision = false;

    _PhysicComp->bMapSlide = true;
    _PhysicComp->bGravity = false;
    _PhysicComp->bFollowOwner = false;

    bEnd = true;
}

void SmokeCloud::Update()
{
    if (bEnd)return;
    Super::Update();

    const float dt = Time::instance().Delta();
    if (bAppear)
    {
        bAppear = false;

        RenderComponent::NotifyType _Notify;
        _RenderComp->Anim(false, true, L"spr_smoke_loop", 6, 0.4f, std::move(_Notify),
            D3DCOLOR_ARGB(Alpha, 255, 255, 255), 0, { 1,1 }, L"Effect", LAYER::ELAYER::EOBJECT_UNDER);
    }
    LoopDuration -= dt;

    if (LoopDuration < 0.f)
    {
        RenderComponent::NotifyType _Notify;
        _Notify[7] = [this]() {
            _RenderComp->bRender = false;
            _CollisionComp->bCollision = false;
            bEnd = true;
        };

        _RenderComp->Anim(false, false, L"spr_smoke_dissolve", 7, 0.4f, std::move(_Notify),
            D3DCOLOR_ARGB(Alpha, 255, 255, 255), 0, { 1,1 }, L"Effect", LAYER::ELAYER::EOBJECT_UNDER);
    };
}

void SmokeCloud::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo);

    auto spTarget = _CollisionInfo._Target.lock();

    if (spTarget)
    {
        if (!spTarget->bSmoke)
        {
            spTarget->bSmoke = true;

            Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [wpTarget = _CollisionInfo._Target]() {
                if (wpTarget.expired())return true;
                wpTarget.lock()->bSmoke = false;
                return true; });
        }
    }

    if (_CollisionInfo._TAG== OBJECT_TAG::ETAG::EEFFECT)
    {
        auto EftRef  = _CollisionInfo._Variable._Cast<std::reference_wrapper<EffectInfo>>();

        EftRef->get().bSmoke = true;
    }

    if (!bHit)
    {
        if (_CollisionInfo._TAG == OBJECT_TAG::CHARCTER ||
            _CollisionInfo._TAG == OBJECT_TAG::EENEMY)
        {
            bHit = true;
            LoopDuration = 5.f;
        }
    }
}

void SmokeCloud::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
    Super::MapHit(_CollisionInfo);

    const float dt = Time::instance().Delta();

    if (_CollisionInfo._ID == OBJECT_ID::ETILE)
    {
      /*  TileHitTime += dt;
        if (TileHitTime > 1.f)
        {
            _PhysicComp->ForceClear();    
        }*/
    }
}

void SmokeCloud::Start()
{
    if (!bEnd)return;

    RenderComponent::NotifyType _Notify;
    _Notify[3] = [this]() {
        bAppear = true;
    };

    _RenderComp->Anim(false, false, L"spr_smoke_appear", 3, 0.4f, std::move(_Notify),
        D3DCOLOR_ARGB(Alpha, 255, 255, 255), 0.f, { 1,1 }, L"Effect", LAYER::ELAYER::EOBJECT_UNDER);
    bAppear = false;
    LoopDuration = 30.f;
    bHit = false;

    _RenderComp->bRender = true;
    _CollisionComp->bCollision = true;
    bEnd = false;
}
