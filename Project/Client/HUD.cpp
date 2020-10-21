#include "stdafx.h"
#include "HUD.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "RenderComponent.h"

void HUD::Initialize() & noexcept
{
	UI::Initialize();

    _TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
    _TransformComp->Scale *= 3.f;

    _RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
    _RenderComp->_RenderInfo.Number = 0;
    _RenderComp->_RenderInfo.ObjectKey = L"Dragon";
    _RenderComp->_RenderInfo.StateKey = L"spr_hud_dragon";
    _RenderComp->_RenderInfo._Layer = LAYER::EUI;

    auto wpThis = _This;
}

void HUD::Update()
{
    if (!_TransformComp)return;

    _TransformComp->Position = global::CameraPos;
    _TransformComp->Position.x += 1920.f/ 2.f;
    _TransformComp->Position.y += 69.f / 2.f;
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
