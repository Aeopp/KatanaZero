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
	RenderManager::instance()._Terrain.CurrentTileTextureStateKey = L"Bunkermansion";
	RenderManager::instance()._Terrain.LoadTilesCurrentStateKeyOnly(LoadMapTexInfoFilePath);
	CollisionTileManager::instance().LoadCollisionTile(LoadCollisionTileInfoFilePath);
	CollisionLineManager::instance().LoadCollisionLine(LoadLineInfoFilePath);
}

void Scene::Release()
{
	for (auto& obj : manage_objs)
	{
		if (!obj)continue;
		obj->bDie = true;
	}
	manage_objs.clear();
}

