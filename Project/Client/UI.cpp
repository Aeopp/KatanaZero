#include "stdafx.h"
#include "UI.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
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
}

void UI::LateUpdate()
{
    object::LateUpdate();
}
