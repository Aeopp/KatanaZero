#include "stdafx.h"
#include "CollisionManager.h"
#include "CollisionLineManager.h"
#include "CollisionTileManager.h"

void CollisionManager::Update()
{
	CollisionTileManager::instance().Update();
	CollisionLineManager::instance().Update();
}

void CollisionManager::Initialize()
{

}

void CollisionManager::Render()
{
	CollisionTileManager::instance().DebugRender();
	CollisionLineManager::instance().DebugRender();
}

void CollisionManager::LateUpdate()
{
	CollisionTileManager::instance().LateUpdate();
	CollisionLineManager::instance().LateUpdate();
}
