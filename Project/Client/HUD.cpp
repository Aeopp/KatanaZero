#include "stdafx.h"
#include "HUD.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"

void HUD::Initialize() & noexcept
{
	UI::Initialize();

    _RenderComp = ComponentManager::instance().Insert<UIRenderComponent>(_This);
    _TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);

    _RenderComp->Depth = 0;

    _TransformComp->Scale  *=3.f;
    
    _RenderComp->_Info.AnimSpeed = 1.f;
    _RenderComp->_Info.bLoop = false;
    _RenderComp->_Info.End = 1;
    _RenderComp->_Info.ObjectKey = L"Dragon";
    _RenderComp->_Info.StateKey = L"spr_hud_dragon";
    _RenderComp->_Info._Layer = LAYER::EUI;
  

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
