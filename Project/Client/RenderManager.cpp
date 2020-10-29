#include "stdafx.h"
#include "RenderManager.h"
#include "global.h"
#include "CollisionLineManager.h"
#include "CollisionTileManager.h"
#include "ComponentManager.h"
#include "UIRenderComponent.h"
#include "GraphicDevice.h"
#include "math.h"
#include <sstream>
#include "InputManager.h"
#include "ObjectManager.h"
#include "Player.h"


// 정적 람다는 가변적인 변수는 절대 캡쳐하지마세요.
static auto YSort = [](std::shared_ptr<RenderComponent>& Lhs, std::shared_ptr<RenderComponent>& Rhs)
{
	return *Lhs->CalcY() < *Rhs->CalcY();
};

static auto LayerSort = [](const std::shared_ptr<RenderComponent>& Lhs, const std::shared_ptr<RenderComponent>& Rhs)
{
	return Lhs->_Info._Layer < Rhs->_Info._Layer;
};

static auto UIDepthSort = [](std::shared_ptr<UIRenderComponent>& Lhs, std::shared_ptr<UIRenderComponent>& Rhs)
{
	return Lhs->Depth < Rhs->Depth;
};

void RenderManager::Render()
{	
	_Terrain.Render();

	auto& RenderCompVec = ComponentManager::instance().Find<RenderComponent>();
	std::sort(std::begin(RenderCompVec), std::end(RenderCompVec),YSort);
	std::stable_sort(std::begin(RenderCompVec), std::end(RenderCompVec), LayerSort);

	for (auto& _RenderComp : RenderCompVec)
	{
		if (_RenderComp->_Control.bRender)
			_RenderComp->_Control._Render(*_RenderComp);
		else 
			_RenderComp->Render();
	}

	auto& UIRenderCompVec = ComponentManager::instance().Find<UIRenderComponent>();
	std::sort(std::begin(UIRenderCompVec), std::end(UIRenderCompVec), UIDepthSort);

	for (auto& _UIRenderComp : UIRenderCompVec)
	{
		if (_UIRenderComp->_Control.bRender)
			_UIRenderComp->_Control._Render(*_UIRenderComp);
		else
			_UIRenderComp->Render();
	}

	DebugMouseInfoRender();
}

void RenderManager::LateUpdate()
{

}

void RenderManager::Release() & noexcept
{

}

void RenderManager::Initialize() & noexcept
{
	InputManager::instance().EventRegist([]()
	{
		RenderManager::instance()._Terrain.bDebugGridRender = !RenderManager::instance()._Terrain.bDebugGridRender;
	}, 'I', InputManager::EKEY_STATE::DOWN)->bFree = true;
}

void RenderManager::DebugMouseInfoRender() & noexcept
{
	if (!global::bDebug)return;

	GraphicDevice::instance().GetSprite()->End();

	RECT _DrawRt;
	_DrawRt.left = global::MousePosScreen.x;
	_DrawRt.right = global::MousePosScreen.x + 350;
	_DrawRt.top= global::MousePosScreen.y+50;
	_DrawRt.bottom = global::MousePosScreen.y + 75;

	std::wstringstream wss;
	wss << L"ScreenMousePos : X:" << global::MousePosScreen.x << L" Y:" << global::MousePosScreen.y << std::endl;
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, wss.str().c_str(),
		wss.str().size(), &_DrawRt, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	wss.str(L"");
	wss.clear();

	_DrawRt;
	_DrawRt.top+= 25;
	_DrawRt.bottom += 25;
	
	wss << L"WorldMousePos : X:" << global::MousePosWorld.x << L" Y:" << global::MousePosWorld.y << std::endl;
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, wss.str().c_str(),
	wss.str().size(), &_DrawRt, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	wss.str(L"");
	_DrawRt.top += 25;
	_DrawRt.bottom += 25;

	auto spPlayer =ObjectManager::instance()._Player.lock();
	if (!spPlayer)return;
	vec3 PlayerLocation = spPlayer->_TransformComp->Position;
	vec3 ToMouseDir = PlayerLocation - global::MousePosWorld;
	D3DXVec3Normalize(&ToMouseDir, &ToMouseDir);
	float PI = std::atan2f(ToMouseDir.y, ToMouseDir.x);

	wss << L"ToMouseAngleFromPlayer : " << PI << std::endl;

		GraphicDevice::instance().GetFont()->DrawTextW(nullptr, wss.str().c_str(),
		wss.str().size(), &_DrawRt, 0, D3DCOLOR_ARGB(255, 109, 114, 255));
	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}
