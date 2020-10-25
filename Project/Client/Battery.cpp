#include "stdafx.h"
#include "Battery.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"
#include "Texture_Manager.h"
#include "math.h"
#include "GraphicDevice.h"
#include "Time.h"


static auto BatteryRenderMake = [](int32_t& Count)
{
	 auto BatteryRender=[&Count](Component& _Comp)
	{
		auto& _UIRenderComp = dynamic_cast<UIRenderComponent&>(_Comp);

		auto spOwner = _UIRenderComp._Owner.lock();
		auto spTexInfo = TextureManager::instance().
			Get_TexInfo(_UIRenderComp._Info.ObjectKey,
				_UIRenderComp._Info.StateKey, _UIRenderComp._Info.GetCurFrame());

		if (!spOwner)return;
		if (!spOwner->_TransformComp)return;
		if (!spTexInfo)return;

		const float JoomScale = global::JoomScale;
		vec3 CameraPos = global::CameraPos;

		matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(true);

		for (int32_t i = 0; i < Count; ++i)
		{
			RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width,spTexInfo->ImageInfo.Height };
			vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
				_UIRenderComp._Info._Color);

			MWorld._41 += 15.f ;
		};
	};

	return BatteryRender;
};


void Battery::Initialize() & noexcept
{
	UI::Initialize();

	_RenderComp = ComponentManager::instance().Insert<UIRenderComponent>(_This);
	_TransformComp  = ComponentManager::instance().Insert<TransformComponent>(_This);
	_RenderComp->Depth = 0;

	_TransformComp->Scale*= 3.f;
	_TransformComp->bFollowOwner = false;

	_RenderComp->_Info.AnimSpeed = 1.f;
	_RenderComp->_Info.bLoop = false;
	_RenderComp->_Info.End = 1;
	_RenderComp->_Info.ObjectKey = L"Dragon";
	_RenderComp->_Info.StateKey = L"spr_hud_battery_part_dragon";
	_RenderComp->_Info._Layer = LAYER::EUI;
	_RenderComp->Depth = 1;

	_RenderComp->_Control.bRender = true;
	_RenderComp->_Control._Render = BatteryRenderMake(Count);

	ScreenPos.x = 42.f;
	ScreenPos.y = 34.f;

	std::weak_ptr<object> wpThis = _This;

	// TODO :: 
	Time::instance().TimerRegist(0.f, 1.f, (std::numeric_limits<float>::max)(),
		[wpThis,this](){
			auto spThis =wpThis.lock();
			if (!spThis)return true;
			
			this->Count++;
			this->Count %= 12;

			return false;
		});
	
}

void Battery::Update()
{
	UI::Update();
}

OBJECT_ID::EID Battery::GetID()
{
	return OBJECT_ID::EID::BATTERY;
}

OBJECT_TAG::ETAG Battery::GetTag()
{
	return Super::GetTag();
}

std::wstring_view Battery::GetName() const&
{
	return L"Battery";
}
