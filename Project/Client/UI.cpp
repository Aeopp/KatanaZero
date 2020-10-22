#include "stdafx.h"
#include "UI.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"
#include "ObjectManager.h"

OBJECT_TAG::ETAG UI::GetTag()
{
    return OBJECT_TAG::EUI;
}

void UI::Initialize() & noexcept
{
    object::Initialize();

}

void UI::LateInitialize() & noexcept
{
    object::LateInitialize();
}

void UI::Release() & noexcept
{
    object::Release();
}

void UI::Update()
{
    object::Update();

    if (!_TransformComp)return;

    _TransformComp->Position = global::CameraPos;
    _TransformComp->Position.x += ScreenPos.x;
    _TransformComp->Position.y += ScreenPos.y;
}

void UI::LateUpdate()
{
    object::LateUpdate();
}
