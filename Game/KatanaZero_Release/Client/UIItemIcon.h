#pragma once
#include "UI.h"

enum EItem : int8_t
{
    None,
    Knife,
    Smoke,
    Explosive,
    Katana,
    End,
};

class UIItemIcon :
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

    void SetIcon(EItem _Item);

    std::pair<int8_t, int8_t > ItemIcons{ Katana,None };
private:
};

