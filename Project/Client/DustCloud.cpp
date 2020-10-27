#include "stdafx.h"
#include "DustCloud.h"
#include "PhysicTransformComponent.h"
#include "RenderComponent.h"
#include "ComponentManager.h"
#include "CollisionComponent.h"
#include "ObjectManager.h"

OBJECT_ID::EID DustCloud::GetID()
{
    return OBJECT_ID::EID::DustCloud;
}

OBJECT_TAG::ETAG DustCloud::GetTag()
{
    return OBJECT_TAG::ETAG::EEFFECT;
}

std::wstring_view DustCloud::GetName() const&
{
    return L"DustCloud"sv;
}

void DustCloud::Initialize() & noexcept
{
    Super::Initialize();

    _TransformComp->Scale *= 1.f;

    _PhysicComp->Mass = 10.f;
    _RenderComp->Anim(false, true, L"Dustcloud", 7, 0.5f, {},
        D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f },
        L"Dustcloud", LAYER::ELAYER::EEFFECT);

    _RenderComp->AfterImgOff();
    _RenderComp->bRender = false;

    _PhysicComp->Position = vec3{ 0.f ,0.f ,0.f };
    _PhysicComp->Dir = { 1.f,0.f,0.f };
    _PhysicComp->bGravity = false;

    Speed = 200.f;
    MoveGoalTime = 1.f;
}

void DustCloud::Update()
{
    Super::Update();
}

void DustCloud::RenderPush(vec3 Pos, vec3 Dir)
{

}

void RenderPush(vec3 Pos, vec3 Dir)
{

}

