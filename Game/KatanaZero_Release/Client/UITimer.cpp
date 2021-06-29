#include "stdafx.h"
#include "UITimer.h"
#include "TransformComponent.h"
#include "UIRenderComponent.h"
#include "ComponentManager.h"

#include "Time.h"

void UITimer::Initialize() & noexcept
{
	UI::Initialize();
	_RenderComp = ComponentManager::instance().Insert<UIRenderComponent>(_This);
	_TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);

	_TransformComp->Scale *= 3.f;

	_RenderComp->_Info.AnimSpeed = 0;
	_RenderComp->_Info.bLoop = false;
	_RenderComp->_Info.ObjectKey = L"Dragon";
	_RenderComp->_Info.StateKey = L"spr_hud_dragon_timer";
	_RenderComp->_Info._Layer = LAYER::EUI;
	_RenderComp->Depth = 1;

	ScreenPos.x = global::ClientSize.first/2.f +10.f;
	ScreenPos.y = 25.f;

	std::weak_ptr<object> wpThis = _This;

	std::weak_ptr<UIRenderComponent> wpRender = _RenderComp;
	
	//TODO::
	Time::instance().TimerRegist(0.f, 1.f, (std::numeric_limits<float>::max)(),
		[wpRender, this]() {
		auto spRender = wpRender.lock();
		if (!spRender)return true;

	
		{
			spRender->_Info.SrcScale.x = (std::max)
				(spRender->_Info.SrcScale.x - 0.01f, 0.f);
		}

		return false;
	});
}

void UITimer::Update()
{
	UI::Update();
}

OBJECT_ID::EID UITimer::GetID()
{
	return OBJECT_ID::EID::UI_TIMER;
}

OBJECT_TAG::ETAG UITimer::GetTag()
{
	return OBJECT_TAG::EUI;
}

std::wstring_view UITimer::GetName() const&
{
	return L"UITimer"sv;

}
