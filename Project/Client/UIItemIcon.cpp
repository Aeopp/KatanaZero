#include "stdafx.h"
#include "UIItemIcon.h"
#include "TransformComponent.h"
#include "UIRenderComponent.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "Time.h"
#include "ComponentManager.h"

static auto ItemIconRenderMake = [](int8_t& LeftItem, int8_t& RightItem)
{
	auto BatteryRender = [&](Component& _Comp)
	{
		auto& _UIRenderComp = dynamic_cast<UIRenderComponent&>(_Comp);

		auto spOwner = _UIRenderComp._Owner.lock();
		
		if (!spOwner)return;
		if (!spOwner->_TransformComp)return;
	
		vec3 CameraPos = global::CameraPos;

		matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(true);

		for (int32_t i = 0; i < 2; ++i)
		{
			uint8_t AnimNumber = !i ? LeftItem : RightItem; 

			auto spTexInfo = TextureManager::instance().
				Get_TexInfo(_UIRenderComp._Info.ObjectKey,
					_UIRenderComp._Info.StateKey, AnimNumber);
			if (!spTexInfo)continue;

			RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _UIRenderComp._Info.SrcScale.x
				,spTexInfo->ImageInfo.Height*_UIRenderComp._Info.SrcScale.y};

			vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
				_UIRenderComp._Info._Color);

			MWorld._41 += 90.f;
		};
	};

	return BatteryRender;
};

void UIItemIcon::Initialize() & noexcept
{
	UI::Initialize();

	_RenderComp = ComponentManager::instance().Insert<UIRenderComponent>(_This);
	_TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);

	_TransformComp->Scale *= 3.f;
	_TransformComp->bFollowOwner = false;

	_RenderComp->_Info.AnimSpeed = 1.f ;
	_RenderComp->_Info.End = 1.f;
	_RenderComp->_Info.bLoop = false;
	_RenderComp->_Info.ObjectKey = L"Itemicons";
	_RenderComp->_Info.StateKey = L"Itemicons";
	_RenderComp->_Info._Layer = LAYER::EUI;
	_RenderComp->Depth = 1;

	_RenderComp->_Control.bRender = true;
	_RenderComp->_Control._Render = ItemIconRenderMake(ItemIcons.first,
													ItemIcons.second );

	ScreenPos.x = 1760.f;
	ScreenPos.y = 30.f;

	std::weak_ptr<object> wpThis = _This;
	std::weak_ptr<UIRenderComponent> wpRender = _RenderComp;

//	TODO::
	Time::instance().TimerRegist(0.f, 1.f, (std::numeric_limits<float>::max)(),
		[wpRender, this]() {

		auto spRender = wpRender.lock();
		if (!spRender)return true;

		ItemIcons.first++;
		ItemIcons.first %= EItem::End;
		ItemIcons.second++;
		ItemIcons.second %=EItem::End;

		return false;
	});
}

void UIItemIcon::Update()
{
    UI::Update();
}

OBJECT_ID::EID UIItemIcon::GetID()
{
    return OBJECT_ID::EID::UI_ITEMICONS;
}

OBJECT_TAG::ETAG UIItemIcon::GetTag()
{
    return OBJECT_TAG::EUI;
}

std::wstring_view UIItemIcon::GetName() const&
{
    return L"UIItemIcons"sv;
}
