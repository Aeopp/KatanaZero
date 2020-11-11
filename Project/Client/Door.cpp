#include "stdafx.h"
#include "Door.h"

#include "Time.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "PhysicTransformComponent.h"
#include "EffectManager.h"
#include "Texture_Manager.h"
#include "sound_mgr.h"

OBJECT_ID::EID Door::GetID()
{
    return OBJECT_ID::EID::DOOR;
}

OBJECT_TAG::ETAG Door::GetTag()
{
    return OBJECT_TAG::ETAG::INTERACTION_OBJECT;
}

std::wstring_view Door::GetName() const&
{
    return L"Door"sv;
}

void Door::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{

}

void Door::Initialize() & noexcept
{
    _TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent >(_This);

    _RenderComp->SlowColor = D3DCOLOR_ARGB(150, 255, 0, 0);
    _RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 255);
    _RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 255, 0, 0);
    _RenderComp->bRender = true;
    _RenderComp->PositionCorrection = vec3{ 0,0,0 };

    _RenderCompDoorGlow = ComponentManager::instance().Insert<RenderComponent>(_This);
    _RenderCompDoorGlow->SlowColor = D3DCOLOR_ARGB(150, 255, 0, 0);
    _RenderCompDoorGlow->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 255);
    _RenderCompDoorGlow->SlowStartColor = D3DCOLOR_ARGB(255, 255, 0, 0);
    _RenderCompDoorGlow->bRender = true;
    _RenderCompDoorGlow->PositionCorrection = vec3{ 0,0,0 };

    /*_RenderComp->Anim(false, false, L"drum", 1, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        0, { 1,1 }, L"oil_drum", LAYER::ELAYER::EOBJECT);*/

    
    _CollisionComp->_CollisionInfo._ShapeType =
        CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 64;
    _CollisionComp->_CollisionInfo.Width = 80;
 //   _CollisionComp->LocationCorrection = { -20,0,0 };
    _CollisionComp->_Tag = CollisionComponent::ETag::EDoor;
    _CollisionComp->bCollision = true;
 //   _CollisionComp->bLhsSlide = true;

    _TransformComp->Scale *= 2.25f;
    _TransformComp->bMapSlide = true;
    _TransformComp->bFollowOwner = false;
}

void Door::SetUpInitState(int32_t InitKey, float XDir)
{
        Door::State _State= (Door::State)InitKey;

        switch (_State)
        {
        case Door::State::Wood:
            break;
        case Door::State::Iron:
            StateKey += L"_iron";
            break;
        case Door::State::Mansion:
            StateKey += L"_mansion";
            break;
        case Door::State::Club:
            StateKey += L"_club";
            break;
        default:
            break;
        }
        
        _RenderComp->Anim(true, false, StateKey, 1, 0.1f, {} , D3DCOLOR_ARGB(255, 255, 255, 255),
            0, { 1,1 }, L"Door", LAYER::ELAYER::EOBJECT_UNDER);

        _RenderCompDoorGlow->Anim(false, true, L"spr_door_glow", 1, 1.f, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
            0, { 1,1 }, L"Door", LAYER::ELAYER::EOBJECT_UNDER);

        _TransformComp->Position += vec3{ 16,10 ,0 };
}

void Door::Open(float XDir)
{
//    bOpening = true;
    bUse = true;
  /*  _CollisionComp->_CollisionInfo.Width *= 100.f;*/

    RenderComponent::NotifyType _Notify;
    _Notify[3] = [this]()
    {
        bOpening = true;
    };
    _Notify[4] = [this]()
    {  
        _CollisionComp->bCollision = false;
        bOpening = false;
    };
    _Notify[20] = [this]() {
        bOpen = true;
        OpenEnd(); 
    };

    _RenderComp->Anim(true, false, StateKey, 20, 1.5f, std::move(_Notify), D3DCOLOR_ARGB(255, 255, 255, 255),
        1, { 1,1 }, L"Door", LAYER::ELAYER::EOBJECT);

    SOUNDPLAY("doorkick", 1.f, false);
    SOUNDPLAY("dooropen", 1.f, false);

    if (XDir < 0.f)
    {
        this->XDir = -1.f;

        _RenderComp->AnimDir = -1.f;
        _RenderComp->PositionCorrection.x += (-37 * _TransformComp->Scale.x);
    }
}

void Door::OpenEnd()
{
    bOpening = false;
    _CollisionComp->bCollision = false;
    _RenderCompDoorGlow->bRender = false;
}
