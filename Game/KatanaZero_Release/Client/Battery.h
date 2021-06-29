#pragma once
#include "UI.h"

class Battery :
    public UI
{
public :
    using Super = UI;
public :
    void Initialize() & noexcept override;
    void Update()override;

    OBJECT_ID::EID   GetID() override;
    OBJECT_TAG::ETAG GetTag()override;
    std::wstring_view GetName()const& override;
    int32_t Count = 11;
    float Time = 11;

    bool IsUse() { return Count > 0; };
private :
};

