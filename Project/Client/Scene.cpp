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
	const std::wstring MapStateKey = L"Bunkermansion";

	TextureManager::instance().LoadTexturesFromTexInfoFile(LoadTextureFilePath);
	RenderManager::instance()._Terrain.CurrentTileTextureStateKey = MapStateKey;
	CollisionTileManager::instance().CurrentStateKey = MapStateKey;
	CollisionLineManager::instance().CurrentStateKey = MapStateKey;
	RenderManager::instance()._Terrain.LoadTilesCurrentStateKeyOnly(LoadMapTexInfoFilePath);
	CollisionTileManager::instance().LoadCollisionTile(LoadCollisionTileInfoFilePath);
	CollisionLineManager::instance().LoadCollisionLine(LoadLineInfoFilePath);
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

