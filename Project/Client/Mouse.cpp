#include "stdafx.h"
#include "Mouse.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"
#include "RenderManager.h"
#include "GraphicDevice.h"
#include "Texture_Manager.h"

static auto MouseRender = [](Component& _Comp)
{
	auto& _UIRenderComp = dynamic_cast<UIRenderComponent&>(_Comp);

	auto spOwner = _UIRenderComp._Owner.lock();
	auto spTexInfo = TextureManager::instance().Get_TexInfo(_UIRenderComp._RenderInfo.ObjectKey,
		_UIRenderComp._RenderInfo.StateKey, _UIRenderComp._RenderInfo.Number);

	if (!spOwner)return;
	if (!spOwner->_TransformComp)return;
	if (!spTexInfo)return;

	const float JoomScale = global::JoomScale;
	vec3 CameraPos = global::CameraPos;

	matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(true);

	MWorld = MWorld * math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

	const auto LocalPoints = math::GetLocalRect(vec2{ (float)spTexInfo->ImageInfo.Width,(float)spTexInfo->ImageInfo.Height });

	bool IsRenderable = false;

	for (const auto& LocalPoint : LocalPoints)
	{
		vec3 WorldPoint{ 0,0,0 };
		D3DXVec3TransformCoord(&WorldPoint, &LocalPoint, &MWorld);
		IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), WorldPoint);
		if (IsRenderable)break;
	}

	//////////////////
	if (IsRenderable)
	{
		RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _UIRenderComp._RenderInfo.SrcScale.x,
							  spTexInfo->ImageInfo.Height * _UIRenderComp._RenderInfo.SrcScale.y };
		vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
		GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
		GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
			_UIRenderComp._RenderInfo._Color);
	}
};


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

	_RenderComp->_Control.bRender = true;
	_RenderComp->_Control._Render = MouseRender;
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
