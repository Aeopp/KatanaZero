#include "stdafx.h"
#include "AttackSlash.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"

OBJECT_ID::EID Attack_Slash::GetID()
{
    return OBJECT_ID::EID::ATTACK_SLASH; 
}

OBJECT_TAG::ETAG Attack_Slash::GetTag()
{
    return OBJECT_TAG::ETAG::PLAYER_ATTACK;
}

std::wstring_view Attack_Slash::GetName() const&
{
    return L"AttackSlash"sv;
}

void Attack_Slash::Initialize() & noexcept
{
    Super::Initialize();
    Speed = 200.f;
    MoveGoalTime = 0.5f;

    _TransformComp->Scale *= 1.f;
    _PhysicComp->Mass = 100.f;

    RenderComponent::NotifyType _Notify;
    _Notify[5] = [this]() 
    {
        bSlashEffectEnd = true;
    };
    _RenderComp->Anim(false, true, L"spr_dragon_slash", 5, 0.3f, std::move(_Notify),
    D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, { 1.f,1.f }, L"Dragon", LAYER::ELAYER::EEFFECT);
    _RenderComp->AfterImgOff(); 
    _RenderComp->bRender = false;

    _CollisionComp->bCollision = false;
    _CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 30;
    _CollisionComp->_CollisionInfo.Width = 25;
    _CollisionComp->PushForce = 500.f;

    _PhysicComp->bGravity = false;
    _PhysicComp->Position = vec3{ 0.f ,0.f ,0.f };
    _PhysicComp->Dir = { 0.f,0.f,0.f };
    _PhysicComp->Scale = { 1.f,1.f,0.f };

    _TransformComp->bFollowOwner = true;

    Speed = 0.f;
    MoveGoalTime = 0.f;
}

void Attack_Slash::LateInitialize() & noexcept
{
    Super::LateInitialize();
}

void Attack_Slash::Release() & noexcept
{
    Super::Release();
}

void Attack_Slash::Update()
{
    Super::Update();
}

void Attack_Slash::LateUpdate()
{
    Super::LateUpdate();

    if (bSlashEffectEnd)
    {
        bSlashEffectEnd = false;
        _RenderComp->bRender = false;
        _CollisionComp->bCollision = false;
    }
}

void Attack_Slash::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
    Super::MapHit(_CollisionInfo);
}

void Attack_Slash::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo);
}

void Attack_Slash::Move(vec3 Dir, const float AddSpeed)
{
    Super::Move(Dir, AddSpeed);
}

void Attack_Slash::AttackStart(vec3 AttackPos,vec3 Dir)
{
    RenderComponent::NotifyType _Notify;
    _Notify[5] = [this]()
    {
        bSlashEffectEnd = true;
    };
    _RenderComp->Anim(true, false, L"spr_dragon_slash", 5, 0.3f, std::move(_Notify));
    _RenderComp->bRender = true;
    _CollisionComp->bCollision = true;

    _PhysicComp->Dir = Dir;
    _PhysicComp->Position = std::move(AttackPos);
    _CollisionComp->CurrentPushDir = Dir;

    D3DXVec3Normalize(&Dir, &Dir);
    _PhysicComp->Rotation.z = atan2f(Dir.y, Dir.x);
}
