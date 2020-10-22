#include "stdafx.h"
#include "Mouse.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"
#include "RenderManager.h"

OBJECT_ID::EID Mouse::GetID()
{
    return OBJECT_ID::MOUSE;
}

OBJECT_TAG::ETAG Mouse::GetTag()
{
    return OBJECT_TAG::ETAG::EUI;
}

std::wstring_view Mouse::GetName() const&
{
    return L"Mouse"sv;
}

void Mouse::Initialize() & noexcept
{
    object::Initialize();
    _TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
    _TransformComp->Scale *= 2.f;

    _RenderComp = ComponentManager::instance().Insert<UIRenderComponent>(_This);
    _RenderComp->_RenderInfo.Number = 0;
    _RenderComp->_RenderInfo.StateKey = L"Cursor";
    _RenderComp->_RenderInfo.ObjectKey = L"Cursor";
    _RenderComp->Depth = (std::numeric_limits<int32_t>::max)();
}

void Mouse::Update()
{
    object::Update();

    POINT mousept;

    GetCursorPos(&mousept);
    ScreenToClient(global::hWND, &mousept);

    global::MousePosScreen.x = mousept.x;
    global::MousePosScreen.y = mousept.y;

    mousept.x -= (global::ClientSize.first / 2.f);
    mousept.y -= (global::ClientSize.second / 2.f);

    mousept.x /= global::JoomScale;
    mousept.y /= global::JoomScale;
   
    _TransformComp->Position.x  = global::MousePosWorld.x=(global::ClientSize.first / 2.f) + mousept.x + global::CameraPos.x;
    _TransformComp->Position.y = global::MousePosWorld.y=(global::ClientSize.second / 2.f) + mousept.y + global::CameraPos.y;
}
