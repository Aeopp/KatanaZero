#include "stdafx.h"
#include "HUD.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"

void HUD::Initialize() & noexcept
{
	UI::Initialize();

    _TransformComp->Scale  *=3.f;
    
    _RenderComp->_RenderInfo.Number = 0;
    _RenderComp->_RenderInfo.ObjectKey = L"Dragon";
    _RenderComp->_RenderInfo.StateKey = L"spr_hud_dragon";
    _RenderComp->_RenderInfo._Layer = LAYER::EUI;
    _RenderComp->Depth = 0;

    ScreenPos.x = 1920.f / 2.f;
    ScreenPos.y = 69.f / 2.f;

    auto wpThis = _This;
}

void HUD::Update()
{
    UI::Update();
}

OBJECT_ID::EID HUD::GetID()
{
    return OBJECT_ID::EID::HUD;
}

OBJECT_TAG::ETAG HUD::GetTag()
{
    return OBJECT_TAG::ETAG::EUI;
}

std::wstring_view HUD::GetName() const&
{
    return L"Hud";
}
