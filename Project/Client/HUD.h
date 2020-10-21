#pragma once
#include "UI.h"
class HUD :
    public UI
{
public :
    void Initialize() & noexcept  override;
    void Update()override;

    OBJECT_ID::EID   GetID() override;
    OBJECT_TAG::ETAG GetTag()override;
    std::wstring_view GetName()const& override;
private:
};

