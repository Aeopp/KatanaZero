#include "stdafx.h"
#include "Scene.h"
#include "object.h"
#include "RenderManager.h"
#include "CollisionTileManager.h"
#include "CollisionLineManager.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"


Scene::~Scene() noexcept
{
	Release();
}

void Scene::Render()
{

}

void Scene::Update( )
{
	
}


void Scene::LateUpdate()
{

}

void Scene::Initialize()
{
	TextureManager::instance().LoadTexturesFromTexInfoFile(LoadTextureFilePath);
	RenderManager::instance()._Terrain.CurrentTileTextureStateKey = CurrentMapStateKey;
	CollisionTileManager::instance().CurrentStateKey = CurrentMapStateKey;
	CollisionLineManager::instance().CurrentStateKey = CurrentMapStateKey;
	RenderManager::instance()._Terrain.LoadTilesCurrentStateKeyOnly(LoadMapTexInfoFilePath);
	CollisionTileManager::instance().LoadCollisionTile(LoadCollisionTileInfoFilePath);
	CollisionLineManager::instance().LoadCollisionLine(LoadLineInfoFilePath);
	CollisionTileManager::instance().SetUpWallRideTile();
}

void Scene::Release()
{
	for (auto& obj : SceneManageObjs)
	{
		if (!obj)continue;
		obj->bDie = true;
	}
	SceneManageObjs.clear();
}

