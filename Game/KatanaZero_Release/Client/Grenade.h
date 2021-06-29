#pragma once
#include "AttackObject.h"

class Grenade :
    public AttackObject
{
private:
    enum class State :uint8_t
    {
        Movement,
        Ignition,
        AfterExplosion,
    };
public:
    using Super = AttackObject;
public:
    virtual OBJECT_ID::EID   GetID()            override;
    virtual OBJECT_TAG::ETAG GetTag()           override;
    virtual std::wstring_view GetName()const& override;
public:
    virtual void Initialize() & noexcept override;
    virtual void Update()override;
    virtual void LateUpdate()override;

    void MapHit(typename math::Collision::HitInfo _CollisionInfo)override;
public:
    void End();
    void Shoot(const vec3& InitLocation, const vec3& InitDir,
        const float Speed);
    void Ignite();
    void Explosion();

    // 1초후에 원 반지름 최대
    // 1초후에 터짐

    static constexpr inline float StartRadius = 25.f;
    static constexpr inline float RadiusGoal = 200.f;
    static constexpr inline float IgniteTime = 2.f;
    static constexpr inline int32_t ExplosionNum = 55;
    static inline constexpr int32_t  CirclePointCount = 512;

    bool bCircleFade = false;
    std::set<int32_t> CircleDrawSet;
    std::array<vec2, CirclePointCount *2+2> CirclePts;

    float CurRadius = StartRadius;

    float CurIgniteTime = IgniteTime;
    float MovementTime = 2.f;
    float Friction = 0.4f;
    Grenade::State _CurState = Grenade::State::AfterExplosion;

    bool bExplosionSound = false;
};

