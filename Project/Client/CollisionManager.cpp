#include "stdafx.h"
#include "CollisionManager.h"
#include "CollisionLineManager.h"
#include "CollisionTileManager.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"

void CollisionManager::Update()
{
	CollisionTileManager::instance().Update();
	CollisionLineManager::instance().Update();

	auto& CollisionCompVec = ComponentManager::instance().Find<CollisionComponent>();

	for (auto& _CollisionComp : CollisionCompVec)
	{
		if (_CollisionComp->_Control.bUpdate)
			_CollisionComp->_Control._Update(*_CollisionComp);
		else
			_CollisionComp->Update();
	}
}

void CollisionManager::Initialize()
{
	CollisionTileManager::instance().Initialize();
}

void CollisionManager::Render()
{
	CollisionTileManager::instance().DebugRender();
	CollisionLineManager::instance().DebugRender();

	auto& CollisionCompVec = ComponentManager::instance().Find<CollisionComponent>();

	for (auto& _CollisionComp : CollisionCompVec)
	{
		if (_CollisionComp->_Control.bRender)
			_CollisionComp->_Control._Render(*_CollisionComp);
		else
			_CollisionComp->Render();
	}
}

void CollisionManager::LateUpdate()
{
	CollisionTileManager::instance().LateUpdate();
	CollisionLineManager::instance().LateUpdate();
}
