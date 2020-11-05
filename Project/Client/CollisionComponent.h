#pragma once
#include "Component.h"

class CollisionComponent :
    public Component
{
public:
    enum class ETag : uint8_t
    {
        ENone,
        ESmoke_Cloud,
        EInteractionObject,
        ETrigger,
        EEnemy,
        EPlayer,
        EPlayerAttack,
        EEnemyAttack,
        ECollisionEffect,
        EItem,
    };
    void Render()override;
    void Update()override;

    std::array<vec3, 4ul> GetWorldRectPt() & noexcept;
    
    struct CollisionInfo
    {
        enum class EShapeType :uint8_t
        {
           Circle,
           Rect,
        };
        // �ݵ�� �������ּ���.
        float Width{ 0.f };
        float Height{ 0.f };
        vec3 Pos{ 0.f,0.f,0.f };
        EShapeType _ShapeType = EShapeType::Circle;
        D3DCOLOR _Color = D3DCOLOR_ARGB(255, 127, 64, 200);
        ////////////////////////////////
    };

    float PushForce = 0.f;
    vec3 CurrentPushDir{ +1.f,0.f,0.f };
    ETag _Tag = ETag::ENone;
    bool bDownJump = false;
    bool bCollision = true;
    CollisionInfo _CollisionInfo{};
};

