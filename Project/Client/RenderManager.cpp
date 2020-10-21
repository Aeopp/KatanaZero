#include "stdafx.h"
#include "RenderManager.h"
#include "global.h"
#include "CollisionLineManager.h"
#include "CollisionTileManager.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "GraphicDevice.h"
#include "math.h"
#include <sstream>


void RenderManager::Render()
{	
	_Terrain.Render();

	 //auto& RenderCompVec = 	ComponentManager::instance().Find<RenderComponent>();








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

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

}
