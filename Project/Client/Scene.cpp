#include "stdafx.h"
#include "Scene.h"
#include "object.h"
#include "RenderManager.h"
#include "CollisionTileManager.h"
#include "CollisionLineManager.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "AStarManager.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Grunt.h"
#include "Gangster.h"
#include "TransformComponent.h"
#include "GO.h"






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
	RenderManager::instance()._Terrain.CurrentTileTextureStateKey = CurrentMapStateKey;
	CollisionTileManager::instance().CurrentStateKey = CurrentMapStateKey;
	CollisionLineManager::instance().CurrentStateKey = CurrentMapStateKey;
	RenderManager::instance()._Terrain.LoadTilesCurrentStateKeyOnly(LoadMapTexInfoFilePath);
	CollisionTileManager::instance().LoadCollisionTile(LoadCollisionTileInfoFilePath);
	CollisionLineManager::instance().LoadCollisionLine(LoadLineInfoFilePath);
	AStarManager::instance().Load(LoadAStarInfoFilePath);
	LoadObjectInfo();
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

void Scene::LoadObjectInfo()
{
	std::wifstream file_output(LoadObjectInfoFilePath);
	size_t _InfoSize{};
	file_output >> _InfoSize;

	for (size_t i = 0; i < _InfoSize; ++i)
	{
		ObjectInfo _ObjectInfo;
		file_output >> _ObjectInfo;
		ObjectInfo::ESpawnID _SpawnID = (ObjectInfo::ESpawnID)_ObjectInfo._ObjectID;
		std::shared_ptr<class object> _SpawnObj;

		switch (_SpawnID)
		{
		case ObjectInfo::Explosive:
			break;
		case ObjectInfo::HeadHunter:
			break;
		case ObjectInfo::Grunt:
			_SpawnObj = ObjectManager::instance().InsertObject<Grunt>
				(_ObjectInfo.Position);
			std::dynamic_pointer_cast<Grunt>(_SpawnObj)->SetUpInitState(_ObjectInfo.XDir, _ObjectInfo.InitState);
			SceneManageObjs.push_back(_SpawnObj);
			break;
		case ObjectInfo::Gangster:
			_SpawnObj = ObjectManager::instance().InsertObject<Gangster>
				(_ObjectInfo.Position);
			std::dynamic_pointer_cast<Gangster>(_SpawnObj)->SetUpInitState(_ObjectInfo.XDir, _ObjectInfo.InitState);
			SceneManageObjs.push_back(_SpawnObj);
			break;
		case ObjectInfo::Knife:
			break;
		case ObjectInfo::Pomp:
			break;
		case ObjectInfo::Shotgun:
			break;
		case ObjectInfo::Smoke:
			break;
		case ObjectInfo::EndTrigger:
			_SpawnObj = ObjectManager::instance().InsertObject<GO>();
			_SpawnObj->_TransformComp->Position = _ObjectInfo.Position;
			std::dynamic_pointer_cast<GO>(_SpawnObj)->SetUpInitState(_ObjectInfo.XDir, NULL);
			SceneManageObjs.push_back(_SpawnObj);
			break;
		case ObjectInfo::Player:
			_SpawnObj = ObjectManager::instance().InsertObject<Player>
				(_ObjectInfo.Position);
			ObjectManager::instance()._Player = std::dynamic_pointer_cast<Player>(_SpawnObj);
			SceneManageObjs.push_back(_SpawnObj);
			break;
		default:
			break;
		}
	}
}

