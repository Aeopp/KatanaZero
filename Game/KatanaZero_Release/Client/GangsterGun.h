#pragma once
#include "object.h"

class GangsterGun :
    public object
{
public:
    virtual OBJECT_ID::EID   GetID()override;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual std::wstring_view GetName()const& override;
    std::shared_ptr<class RenderComponent> _RenderComp;
    std::shared_ptr<class Player> _Target;

    using Super = object;
    virtual void Initialize() & noexcept override;
    virtual void Update()override;
    void GunRenderStart();
    void GunRenderEnd();
    void Attack();
private:
};

