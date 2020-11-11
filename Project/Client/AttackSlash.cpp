#include "stdafx.h"
#include "AttackSlash.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "EffectManager.h"
#include "Time.h"
#include "sound_mgr.h"
#include "ObjectManager.h"
#include "Camera.h"

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

    _OldSlash1 = ComponentManager::instance().Insert<RenderComponent>(_This);
    _OldSlash2 = ComponentManager::instance().Insert<RenderComponent>(_This);

    RenderComponent::NotifyType _Notify;
    _Notify[5] = [this]() 
    {
        bSlashEffectEnd = true;
    };

    _RenderComp->Anim(false, true, L"spr_master_slash", 5, 0.3f, std::move(_Notify),
    D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, { 1.f,1.f }, L"Dragon", LAYER::ELAYER::EEFFECT);
    _RenderComp->AfterImgOff(); 
    _RenderComp->bRender = false;

    {
        _OldSlash1->Anim(false, true, L"spr_oldslash", 13, 0.3f, {},
            D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, { 1.f,1.f }, L"Dragon", LAYER::ELAYER::EEFFECT);
        _OldSlash1->AfterImgOff();
        _OldSlash1->bRender = false;

        _OldSlash2->Anim(false, true, L"spr_oldslash2", 7, 0.3f, {},
            D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, { 1.f,1.f }, L"Dragon", LAYER::ELAYER::EEFFECT);
        _OldSlash2->AfterImgOff();
        _OldSlash2->bRender = false;
    }

    _CollisionComp->bCollision = false;
    _CollisionComp->_Tag = CollisionComponent::ETag::EPlayerAttack;
    _CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 50;
    _CollisionComp->_CollisionInfo.Width = 33;
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
        _RenderComp->_AfterImgVec.clear();
        _RenderComp->AfterImgOff();

        _CollisionComp->bCollision = false;

        {
            _OldSlash1->bRender = false;
            _OldSlash1->_AfterImgVec.clear();
            _OldSlash1->AfterImgOff();

            _OldSlash2->bRender = false;
            _OldSlash2->_AfterImgVec.clear();
            _OldSlash2->AfterImgOff();
        }
    }
}

void Attack_Slash::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
    Super::MapHit(_CollisionInfo);
}

void Attack_Slash::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    Super::Hit(_Target, _CollisionInfo);

    if (_CollisionInfo._ID == OBJECT_ID::EID::BULLET)
    {
        auto _RefEftInfo = _CollisionInfo._Variable._Cast<std::reference_wrapper<EffectInfo>>();
        if (_ReflectBulletIDs.contains(_RefEftInfo->get()._ID))return;

        vec3 initDir = -_RefEftInfo->get().Dir;
        D3DXVec3Normalize(&initDir, &initDir);

        constexpr float BulletSpeed = 2300.f;
        /*
        EffectManager::instance().EffectPush(L"Effect", L"spr_bulletreflect",
            5, 0.13f, 5 * 0.13f + 0.01f, OBJECT_ID::EID::ENONE, true, _PhysicComp->Position,
            { 0,0,0 }, { 2.25,2.25,0 }, false, false, false, false, 0, 0, 255, false, 0,
            0);*/
        RAND_SOUNDPLAY("sound_bullethit", { 1,3 }, 1.f);



        float ImpactRotZ = atan2f(-initDir.y, -initDir.x);

        EffectManager::instance().EffectPush(L"Effect",
            L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
            _CollisionInfo.Position + -initDir * 77,
            { 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
            255, false, 0, ImpactRotZ, 0, 0);

        EffectManager::instance().EffectPush(L"Effect",
            L"spr_slashfx", 5, 0.08f,
            0.08f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false, _CollisionInfo.Position,
            { 0,0,0 }, { 2.9,2.9,0 });

        ObjectManager::instance()._Camera.lock()->CameraShake(
            500.f, initDir, 0.3f);

        EffectManager::instance().EffectPush(L"Effect",L"spr_bulletreflect",
            5, 0.13f,5* 0.13f +0.01f,OBJECT_ID::EID::REFLECT,true,_CollisionInfo.Position,
            {0,0,0}, { 2.25,2.25,0 },false,false,false,false,0,0,255,false,0,
            atan2f(initDir.y, initDir.x));

        sound_mgr::instance().Play("slash_bullet", false, 0.75f);

        EffectManager::instance().EffectPush(L"Effect", L"spr_bullet",
            1, (std::numeric_limits<float>::max)(), 10.f, OBJECT_ID::EID::REFLECT_BULLET,
            true, _CollisionInfo.Position, initDir * BulletSpeed, { 1,1,1 }, false,
            true, false, true, 34, 2, 255, false, 0.f, atan2f(initDir.y, initDir.x),
            0);

        _RefEftInfo->get().T = FLT_MAX - 1.f;
        _RefEftInfo->get().bPhysic = false;

         _ReflectBulletIDs.insert(_RefEftInfo->get()._ID);

         // 슬래쉬 공격 끝
            bSlashEffectEnd = true;
    }
 /*   if (_CollisionInfo._TAG == OBJECT_TAG::ETAG::ENEMY ||
        _CollisionInfo._TAG == OBJECT_TAG::ETAG::ENEMY_ATTACK)
    {
        auto spEnemy = _CollisionInfo._Target.lock();
        if (spEnemy)
        {
            
        
        }
    }*/
}

void Attack_Slash::Move(vec3 Dir, const float AddSpeed)
{
    Super::Move(Dir, AddSpeed);
}

void Attack_Slash::DashAttackStart(vec3 AttackPos, vec3 Dir)
{
    RenderComponent::NotifyType _Notify;

   //  RAND_SOUNDPLAY("slash", { 1,3 }, 0.9f, false);

    _Notify[5] = [this]()
    {
        bSlashEffectEnd = true;
    };

    _RenderComp->Anim(true, false, L"spr_master_slash", 5, 0.2f*0.01f, std::move(_Notify));
    _RenderComp->bRender = true;
    _RenderComp->AfterImgOn();

    if (global::_CurGameState == global::ECurGameState::PlaySlow)
    {
        _OldSlash1->Anim(true, false, L"spr_oldslash", 13, 0.3f * 0.01f);
        _OldSlash1->bRender = true;
        _OldSlash1->AfterImgOn();

        _OldSlash2->Anim(true, false, L"spr_oldslash2", 7, 0.3f * 0.01f);
        _OldSlash2->bRender = true;
        _OldSlash2->AfterImgOn();
    }

    _CollisionComp->bCollision = true;

    _PhysicComp->Dir = Dir;
    _PhysicComp->Position = std::move(AttackPos);
    _CollisionComp->CurrentPushDir = Dir;

    D3DXVec3Normalize(&Dir, &Dir);
    _PhysicComp->Rotation.z = atan2f(Dir.y, Dir.x);
}

void Attack_Slash::AttackStart(vec3 AttackPos,vec3 Dir)
{
    RenderComponent::NotifyType _Notify;
    RAND_SOUNDPLAY("slash", { 1,3 }, 0.9f, false);

    _Notify[5] = [this]()
    {
        bSlashEffectEnd = true;
    };
    _RenderComp->Anim(true, false, L"spr_master_slash", 5, 0.20f, std::move(_Notify));
    _RenderComp->bRender = true;
    _RenderComp->AfterImgOn();

       if (global::_CurGameState == global::ECurGameState::PlaySlow)
        {
            _OldSlash1->Anim(true, false, L"spr_oldslash", 13, 0.3f);
            _OldSlash1->bRender = true;
             _OldSlash1->AfterImgOn();


            _OldSlash2->Anim(true, false, L"spr_oldslash2", 7, 0.3f);
            _OldSlash2->bRender = true;
              _OldSlash2->AfterImgOn();
        }

    _CollisionComp->bCollision = true;

    _PhysicComp->Dir = Dir;
    _PhysicComp->Position = std::move(AttackPos);
    _CollisionComp->CurrentPushDir = Dir;

    D3DXVec3Normalize(&Dir, &Dir);
    _PhysicComp->Rotation.z = atan2f(Dir.y, Dir.x);
}
