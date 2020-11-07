#pragma once
#include "object.h"

class Door :
    public object
{
public:
    using Super = object;
    virtual OBJECT_ID::EID   GetID()override;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual std::wstring_view GetName()const& override;
    virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;

    static const inline float XCorrection = -43.f;

    void Initialize() & noexcept override;
    void SetUpInitState(int32_t InitKey, float XDir);

    void Open(float XDir);
    void OpenEnd();

    bool bOpen = false;
    float XDir = 1;

    bool bGlowRender = true;
    bool bOpening = false;
private:
    enum class State :uint8_t
    {
        Wood=1,
        Iron,
        Mansion,
    };
    std::wstring StateKey = L"spr_door_animation";
    std::shared_ptr<class CollisionComponent> _CollisionComp;
    std::shared_ptr<class RenderComponent> _RenderComp;
    std::shared_ptr<class RenderComponent> _RenderCompDoorGlow;
};

