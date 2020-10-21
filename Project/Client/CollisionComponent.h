#pragma once
#include "Component.h"

class CollisionComponent :
    public Component
{
public:
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
        // 반드시 설정해주세요.
        float Width{ 0.f };
        float Height{ 0.f };
        vec3 Pos{ 0.f,0.f,0.f };
        EShapeType _ShapeType = EShapeType::Circle;
        D3DCOLOR _Color = D3DCOLOR_ARGB(255, 127, 64, 200);
        ////////////////////////////////
    };

    CollisionInfo _CollisionInfo{};
};

