#include "stdafx.h"
#include "RenderManager.h"
#include "global.h"
#include "CollisionLineManager.h"
#include "CollisionTileManager.h"

void RenderManager::Render()
{	
	_Terrain.Render();
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
