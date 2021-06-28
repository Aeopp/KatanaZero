#include "stdafx.h"
#include "Grenade.h"
#include "RenderComponent.h"
#include "ObjectManager.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"
#include "Time.h"
#include "ComponentManager.h"
#include "GraphicDevice.h"
#include "EffectManager.h"
#include "sound_mgr.h"


OBJECT_ID::EID Grenade::GetID()
{
    return OBJECT_ID::EID::GRENADE;
}

OBJECT_TAG::ETAG Grenade::GetTag()
{
    return OBJECT_TAG::ETAG::REFELECT;
}

std::wstring_view Grenade::GetName() const&
{
    return L"Grenade"sv;
}

void Grenade::Initialize() & noexcept
{
    Super::Initialize();

    _CollisionComp->_Tag = CollisionComponent::ETag::REFLECT;

    _TransformComp->Scale *= 2.f;
    _PhysicComp->Mass = 0.f;

    _RenderComp->AfterImgOn();
    
    _RenderComp->Anim(false, false, L"Grenade", 1, 1.f, {}, D3DCOLOR_ARGB(255, 255, 255, 255),
        0, { 1,1 }, L"Grenade", LAYER::ELAYER::EOBJECT);
    _RenderComp->bRender = false;
    _CollisionComp->bCollision = false;
    _PhysicComp->bGravity = true;
    _PhysicComp->GravityCoefficient = 0.3f;

    _RenderComp->NormalAfterImgPushDelta = 0.01f;

    _CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
    _CollisionComp->_CollisionInfo.Height = 18;
    _CollisionComp->_CollisionInfo.Width = 14;
    _CollisionComp->PushForce = 300.f;

    _RenderComp->_RenderAfterEvent = [this]()
    {
        if (_CurState != Grenade::State::Ignition)return;
        
        constexpr float LineWidth = 2.f;
        constexpr float _2PI = math::PI;
        constexpr float PointBetWeenAngle = _2PI / (float)CirclePointCount;

        vec3 Center = _PhysicComp->Position;

        std::array<vec2, (int32_t)CirclePointCount *2+2> _PointArr;

        for (uint32_t i = 0; i < CirclePointCount*2+2; ++i)
        {
            vec3 Pos3D = (Center + 
                        vec3{ cosf(PointBetWeenAngle * i) * CurRadius,
                        sinf(PointBetWeenAngle * i) * CurRadius ,0.f });
            vec2 Pos{ Pos3D.x,Pos3D.y };

            _PointArr[i] = std::move(Pos);
            _PointArr[i].x -= global::CameraPos.x;
            _PointArr[i].y -= global::CameraPos.y;

            const auto _CameraJoomMatrix = math::GetCameraJoomMatrix(global::JoomScale,
                vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

            D3DXVec2TransformCoord(&_PointArr[i], &_PointArr[i],&_CameraJoomMatrix);
        }

        GraphicDevice::instance().GetSprite()->End();
        GraphicDevice::instance().GetLine()->SetWidth(LineWidth);

        for (size_t i = 0; i < _PointArr.size()-1; ++i)
        {
            if (CircleDrawSet.contains(i))
            {
                GraphicDevice::instance().GetLine()->Draw(&_PointArr[i], 2, D3DCOLOR_ARGB(255, 255, 255, 0));
            }
        }
        GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
    };
}

void Grenade::Update()
{
    Super::Update();

    const float dt = Time::instance().Delta();
    if (_CurState == State::Movement)
    {
        _TransformComp->Position += _TransformComp->Dir * dt * Speed;
    }
};

void Grenade::LateUpdate()
{
    Super::LateUpdate();
    const float dt = Time::instance().Delta();


    if(_CurState==State::Movement)
    {
        MovementTime -= dt;
     
        if (MovementTime < 0.f)
        {
            Ignite();
        }
        else
        {
           
            _TransformComp->Rotation.z += dt * 5.f;

            if (_PhysicComp->Position.y < 1230)
            {
                vec3 Normal = { 0,1,0 };
                vec3 Reverse = (-_TransformComp->Dir);

                float dot = D3DXVec3Dot(&Reverse, &Normal);

                vec3    Proj = dot * Normal;

                _TransformComp->Dir += (Proj * 2.f);

                D3DXVec3Normalize(&_TransformComp->Dir, &_TransformComp->Dir);

                _TransformComp->Dir *= Friction;
            }
        }
    }
    
    if (_CurState == State::Ignition)
    {
        CurIgniteTime -= dt;

        if (CurIgniteTime < 0.2f&&!bExplosionSound)
        {
            bExplosionSound = true;
            SOUNDPLAY("sound_boss_huntressbomb_armed_01", 0.6f);
        }

        if (CurIgniteTime < 0.f)
        {
            Explosion();
        }
        else
        {
            if (CurIgniteTime > (IgniteTime / 2.f))
            {
                CurRadius = math::lerp(CurRadius, RadiusGoal, IgniteTime - CurIgniteTime);

                // 원의 반지름을 키운다.
            }
            else if (CurIgniteTime < (IgniteTime / 2.f))
            {
                if (!bCircleFade)
                {
                    bCircleFade = true;
                  
                }
                else
                {
                    int32_t Count =( dt * CirclePts.size())*3;

                    for (int32_t i = 0; i < Count; ++i)
                    {
                        CircleDrawSet.erase(math::Rand<int32_t>({ 0, CirclePts.size() - 2 }));
                    }
                    // 반지름은 최고이고 점을 없앤다.
                }
            }
        }
    }
}
void Grenade::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
    Super::MapHit(_CollisionInfo);

    if (_CollisionInfo._ID == OBJECT_ID::ETILE && _CurState==State::Movement)
    {
        vec3 Normal = _CollisionInfo.Normal;
        vec3 Reverse =  (-_TransformComp->Dir);
        
        float dot = D3DXVec3Dot(&Reverse, &Normal);

       vec3    Proj=     dot* Normal;
       
       _TransformComp->Dir += (Proj * 2.f);

       D3DXVec3Normalize(&_TransformComp->Dir, &_TransformComp->Dir);
        
       _TransformComp->Dir *=Friction;
    }
};

void Grenade::End()
{
    _CollisionComp->bCollision = false;
    _RenderComp->bRender = false;
}

void Grenade::Shoot(const vec3& InitLocation, const vec3& InitDir, const float Speed)
{
    _PhysicComp->GravityAcceleration = 0.f;
    _PhysicComp->bGravity = true;
    _PhysicComp->GravityCoefficient = 0.3f;
    _RenderComp->_AfterImgVec.clear();

    _CurState = State::Movement;
    MovementTime = 2.f;
    _RenderComp->bRender = true;
    _CollisionComp->bCollision = true;
    this->Speed = Speed;
    _TransformComp->Position = InitLocation;
    D3DXVec3Normalize(&_TransformComp->Dir, &InitDir);
}

void Grenade::Ignite()
{
    CircleDrawSet.clear();

    _CurState = State::Ignition;
    CurIgniteTime = IgniteTime;
    bCircleFade = false;

    for (int32_t i = 0; i < CirclePts.size() - 1; ++i)
    {
        CircleDrawSet.insert(i);
    }

    SOUNDPLAY("sound_boss_huntressmines_armed_01", 0.75f);

    _PhysicComp->bGravity = false;
    _PhysicComp->GravityAcceleration = 0.f;
    _PhysicComp->ForceClear();

    CurRadius = StartRadius;
}

void Grenade::Explosion()
{
    _CurState = State::AfterExplosion;
    _RenderComp->bRender = false ;
    _CollisionComp->bCollision = false ;

    vec3 Axis = { 1,0,0 };

    SOUNDPLAY("sound_boss_huntress_explosion_01", 1.f);
    
    for (int32_t i = 0; i < ExplosionNum; ++i)
    {
        vec3 Dir = math::RotationVec(Axis, math::Rand<float>({ -360,360 }));
        float Radius = math::Rand<float>({ StartRadius,RadiusGoal });
        const vec3 Location = _TransformComp->Position + Dir * Radius;

        EffectManager::instance().EffectPush(L"Effect", L"spr_explosion_1",
            11, 0.1f, 11 * 0.1f + 0.01f, OBJECT_ID::EID::EXPLOSION, true,
            Location, { 0,0,0 },
            { 3,3,0 }, false, true, false, true, 33, 33,
            255, false, 0.f, 0.f, 0.f, 0);
    }

}
