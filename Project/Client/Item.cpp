#include "stdafx.h"
#include "Item.h"

#include "ObjectManager.h"

#include "Time.h"
#include "NormalEnemy.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "PhysicTransformComponent.h"
#include "Player.h"
#include "EffectManager.h"
#include "SmokeCloud.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SmokeCloud.h"




OBJECT_ID::EID Item::GetID()
{
    return OBJECT_ID::EID::ITEM;
}

OBJECT_TAG::ETAG Item::GetTag()
{
    return OBJECT_TAG::ETAG::ITEM;
}

std::wstring_view Item::GetName() const&
{
    return L"Item"sv;
}

void Item::Initialize() & noexcept
{
    Super::Initialize();
    
    _TransformComp = ComponentManager::instance().Insert<PhysicTransformComponent>(_This);
    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _CollisionComp = ComponentManager::instance().Insert<CollisionComponent >(_This);
    _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_TransformComp);

    _PhysicComp->Scale *= 1.5f;
    _PhysicComp->Mass = 100.f;
    _RenderComp->Anim(true, false, L"spr_butcher_knife", 2, FLT_MAX, {},
        D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"Item", LAYER::ELAYER::EOBJECT);
    _RenderComp->AfterImgOff();
    _RenderComp->PositionCorrection = vec3{ 0,0,0 };
    _RenderComp->SlowDeltaCoefft = 0.05f;
    _RenderComp->NormalAfterImgPushDelta *= 1.0f;
    _RenderComp->bRender = true;

    _CollisionComp->_CollisionInfo._ShapeType =
        CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 30;
    _CollisionComp->_CollisionInfo.Width = 16;
    _CollisionComp->_Tag = CollisionComponent::ETag::EItem;
    _CollisionComp->bCollision = true;
    
    _PhysicComp->bMapSlide = true;
    _PhysicComp->bGravity = false;
    _PhysicComp->bFollowOwner = false;
}

void Item::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
    if (bUse)return;

    Super::Hit(_Target, _CollisionInfo);

    if (_CollisionInfo._ID == OBJECT_ID::EID::EPLAYER && !bEq)
    {
        auto _Player = std::dynamic_pointer_cast<class Player>(_CollisionInfo._Target.lock());
        if (!_Player->IsEqItem())
        {
            _Player->_SpUIItemIcon->SetIcon(_ItemID);
            _Player->EqItem(std::dynamic_pointer_cast<Item> (_This.lock()));
            _RenderComp->bRender = false;

            EffectManager::instance().EffectPush(L"Item", EQStateKey, 2, 0.15f, 2.f, OBJECT_ID::EID::ENONE, false,
                EQEffectPosCorre + _PhysicComp->Position, { 0,0,0 }, { 1.7,1.7,0 }, true, false, false, false, 0, 0, 255, true, 0, 0, 0, 0);

            bEq = true;
        }
        
    };

    if (_CollisionInfo._TAG == OBJECT_TAG::ETAG::EENEMY  &&bThrow)
    {
        auto _Enemy = std::dynamic_pointer_cast<NormalEnemy> (_CollisionInfo._Target.lock());
        if (_Enemy->_EnemyState != NormalEnemy::State::Die)
        {
            if (_ItemID == EItem::Knife)
            {
                _Enemy->_EnemyState = NormalEnemy::State::Die;
            }
            Interaction(std::move(_CollisionInfo));
        }
    }
    else if (_CollisionInfo._ID == OBJECT_ID::EID::BULLET && bThrow)
    {
        Interaction(std::move(_CollisionInfo));
    }
}

void Item::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
    if (bUse)return;

    Super::MapHit(_CollisionInfo);

    if (bThrow)
    {
        Interaction(std::move(_CollisionInfo));
    }
}

void Item::Update()
{
    Super::Update();
    const float dt = Time::instance().Delta();

    if (bThrow)
    {
        CurThrowRotZ += ThrowRotZSpeed * dt;

        _PhysicComp->Rotation.z = CurThrowRotZ;
    }
}

void Item::KnifeInteraction(math::Collision::HitInfo _Info)
{
    BulletInteraction(_Info);

    /*auto _Enemy = std::dynamic_pointer_cast<NormalEnemy> (_Info._Target.lock());
    _Enemy->_EnemyState = NormalEnemy::State::Die;*/
}

void Item::ExplosiveInteraction(math::Collision::HitInfo _Info)
{
    EffectManager::instance().EffectPush(L"Effect", L"spr_explosion_1",
        11, 0.1f, 11 * 0.1f + 0.01f, OBJECT_ID::EID::EXPLOSION, true,
        _PhysicComp->Position, { 0,0,0 },
        { 4,4,0 }, false, true, false, true, 33, 33,
        255, false, 0.f, 0.f, 0.f, 0);

 //   BulletInteraction(_Info);
}

void Item::SmokeInteraction(math::Collision::HitInfo _Info)
{
    //BulletInteraction(_Info);
    
    const vec3 Location = _Info.Position; // -(_Info.PosDir * 100.f);

    for (auto& _spSmoke : _Smoke)
    {
        vec3 Dir = { math::Rand<float>({1,2}),math::Rand<float>({1,2}),0 };
        Dir =math::RotationVec(Dir, math::Rand<float>({ -360,360 }));
      //  D3DXVec3Normalize(&Dir, &Dir);
        _spSmoke->Alpha = math::Rand<int32_t>({ 125,255 });
        _spSmoke->_TransformComp->Position = Location; 
        _spSmoke->_PhysicComp->Move(Dir * math::Rand<float>({ 12,65 }), 0, 1.5f, { 0,0,0 }, 0);
        _spSmoke->_PhysicComp->Mass = 0.f;
        _spSmoke->Start();
    }

  /*  for (int outer = 0; outer < Outer; ++outer)
    {
        for (int inner = 0; inner < Inner; ++inner)
        {
            const float Angle = AngleDiff * inner;
            const float Dis = Distance * Outer;
            vec3 Dir = math::RotationVec({ 1,0,0 }, Angle);
            _Smoke[outer * Inner + inner]->Start();
            _Smoke[outer * Inner + inner]->_TransformComp->Position = Location;
            _Smoke[outer * Inner + inner]->_PhysicComp->Move(Dir * Dis, 0.f, 1.f, Dir, 0.f);
        }
    }*/
}

void Item::BulletInteraction(const math::Collision::HitInfo& _Info)
{
    if (_Info._ID == OBJECT_ID::EID::BULLET)
    {
        auto _RefEftInfo = _Info._Variable._Cast<std::reference_wrapper<EffectInfo>>();
        
        vec3 initDir = -_RefEftInfo->get().Dir;
        D3DXVec3Normalize(&initDir, &initDir);

        constexpr float BulletSpeed = 2000.f;

        EffectManager::instance().EffectPush(L"Effect", L"spr_bulletreflect",
            5, 0.13f, 5 * 0.13f + 0.01f, OBJECT_ID::EID::REFLECT, true, _Info.Position,
            {}, { 2.25,2.25,0 }, false, false, false, false, 0, 0, 255, false, 0,
            atan2f(initDir.y, initDir.x));

        vec3 RefBulletDir= -_Info.PosDir;

        EffectManager::instance().EffectPush(L"Effect", L"spr_bullet",
            1, (std::numeric_limits<float>::max)(), 10.f, OBJECT_ID::EID::REFLECT_BULLET,
            true, _Info.Position, RefBulletDir *BulletSpeed, { 3,1,1 }, false,
            true, false, true, 34, 2, 255, false, 0.f, atan2f(RefBulletDir.y, RefBulletDir.x),
            0);


        _RefEftInfo->get().T = FLT_MAX - 1.f;
        _RefEftInfo->get().bPhysic = false;
    }
}

void Item::SetUpInitState(int32_t ItemID)
{
    // spr_butcher_knife
    // spr_explosive_vial
     // spr_smoke_vial
    _ItemID = (EItem)ItemID;

    switch (_ItemID)
    {
    case None:
        break;
    case Knife:
        _RenderComp->Anim(false, false, L"spr_butcher_knife", 1, FLT_MAX);
        EQStateKey = L"eq_knife";
        break;
    case Smoke:
        _RenderComp->Anim(false, false, L"spr_smoke_vial", 1, FLT_MAX);
        EQStateKey = L"eq_smoke";
        for (auto& spSmoke : _Smoke)
        {
            spSmoke = ObjectManager::instance().InsertObject<SmokeCloud>(); 
        }
        break;
    case Explosive:
        _RenderComp->Anim(false, false, L"spr_explosive_vial", 1, FLT_MAX);
        EQStateKey = L"eq_explosive";
        break;
    case Katana:
        break;
    case End:
        break;
    default:
        break;
    };
}

void Item::Throw(vec3 InitLocation, vec3 Dir)
{
    if (bUse)return;

    const float Rich = 20.f;
    const float Speed = 3000.f;
    _RenderComp->bRender = true;
    
    _PhysicComp->Position = InitLocation;
    _PhysicComp->Move(Dir * Speed, Speed * 0.1f, 10.f, Dir, 0);
    CurThrowRotZ = atan2f(Dir.y, Dir.x);
    _RenderComp->AfterImgOn();

    bThrow = true;
    ThrowDir = Dir;
    D3DXVec3Normalize(&ThrowDir, &ThrowDir);

    if (_ItemID == EItem::Explosive)
    {
        _CollisionComp->CurrentPushDir = vec3{ 0,-1.f,0.f };
        _CollisionComp->PushForce = 700.f;
    }
    else
    {
        _CollisionComp->CurrentPushDir = ThrowDir;
        _CollisionComp->PushForce = 10.f;
    }
}

void Item::Interaction(math::Collision::HitInfo _Info)
{
     // 여기서 아이템마다 고유한 작용
    bUse = true;
    bThrow = false;
    bEq = false;
    _CollisionComp->bCollision = false;
    _RenderComp->bRender = false;
    _RenderComp->AfterImgOff();

    EffectManager::instance().EffectPush(L"Effect",L"spr_blunt_impact",
    7, 0.09f,7* 0.12f + 0.09f,OBJECT_ID::EID::ENONE,true,_PhysicComp->Position,
        { 0,0,0 }, { 2,2,0 },false,false,false,false,0,0,255,false,
        0, atan2f(-ThrowDir.y, -ThrowDir.x), 0, 0);

    switch (_ItemID)
    {
    case None:
        break;
    case Knife:
        KnifeInteraction(std::move(_Info));
        break;
    case Smoke:
        SmokeInteraction(std::move(_Info));
        break;
    case Explosive:
        ExplosiveInteraction(std::move(_Info));
        break;
    case Katana:
        break;
    case End:
        break;
    default:
        break;
    }
}
