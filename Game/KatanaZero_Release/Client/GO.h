#pragma once
#include "UI.h"
class GO :
    public UI
{
public:
    void Initialize() & noexcept  override;
    void Update()override;
    void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;

    OBJECT_ID::EID   GetID() override;
    OBJECT_TAG::ETAG GetTag()override;
    std::wstring_view GetName()const& override;
    
    bool bClear = false;

    float XDir = 1.f;


    float XRange = 100.f;

    vec2 InitScreenPos;

    std::shared_ptr<class CollisionComponent > _CollisionComponent{};
    float ClearCheckTime = 2.f;

    void SetUpInitState(float XDir, int32_t InitState);
};

