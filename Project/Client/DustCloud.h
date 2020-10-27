#pragma once
#include "Effect.h"

struct RenderInfo
{
    vec3 Pos{};
    vec3 Dir{};
    float Speed{};
    float T{};
};

class DustCloud :
    public Effect
{
public :
    using Super = Effect;
public :
    virtual OBJECT_ID::EID   GetID();
    virtual OBJECT_TAG::ETAG GetTag();
    virtual std::wstring_view GetName()const&;
public :
    virtual void Initialize() & noexcept;
    virtual void Update();

    void RenderPush(vec3 Pos,vec3 Dir);
};

