#include "stdafx.h"
#include "Grunt_Slash.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"

OBJECT_ID::EID Grunt_Slash::GetID()
{
    return OBJECT_ID::EID::GRUNT_SLASH;
}

OBJECT_TAG::ETAG Grunt_Slash::GetTag()
{
    return OBJECT_TAG::ETAG::ENEMY_ATTACK;
}

std::wstring_view Grunt_Slash::GetName() const&
{
    return L"GruntSlash"sv;
}

void Grunt_Slash::Update()
{
    Super::Update();
    
        if (bSlashEffectEnd)
        {
            bSlashEffectEnd = false;
            _RenderComp->bRender = false;
            _CollisionComp->bCollision = false;
        }
}

void Grunt_Slash::Initialize() & noexcept
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
    _RenderComp->Anim(false, true, L"spr_gruntslash", 5, 0.3f, move(_Notify),
        D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f },
        L"Grunt", LAYER::ELAYER::EEFFECT);
    _RenderComp->AfterImgOff();
    _RenderComp->bRender = false;

    _CollisionComp->bCollision = false;
    _CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 25;
    _CollisionComp->_CollisionInfo.Width = 25;
    _CollisionComp->PushForce = 200.f;
    _CollisionComp->_Tag = CollisionComponent::ETag::EEnemyAttack;

    _PhysicComp->bGravity = false;
    _PhysicComp->Position = vec3{ 0.f,0.f,0.f };
    _PhysicComp->Dir = { 1.f,0.f,0.f };

    Speed = 0.f;
    MoveGoalTime = 0.f;
}

void Grunt_Slash::LateUpdate()
{
    Super::LateUpdate();

}




void Grunt_Slash::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo); 
}


void Grunt_Slash::AttackStart(vec3 AttackPos, vec3 Dir)
{
    RenderComponent::NotifyType _Notify;
    _Notify[5] = [this]()
    {
        bSlashEffectEnd = true;
    }; 
    _RenderComp->Anim(true, false, L"spr_gruntslash", 5, 0.3f, move(_Notify));
    _RenderComp->bRender = true;
    _CollisionComp->bCollision = true;

    _PhysicComp->Dir = AttackPos;
    _PhysicComp->Position = std::move(AttackPos);
    _CollisionComp->CurrentPushDir = Dir;

    _PhysicComp->Rotation.z = atan2f(Dir.y, Dir.x);
}
