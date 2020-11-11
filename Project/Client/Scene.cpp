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
#include "Item.h"
#include "Drum.h"
#include "Door.h"
#include "sound_mgr.h"


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
		std::shared_ptr<class Door> _Door;

		switch (_SpawnID)
		{
		case ObjectInfo::Explosive:
			_SpawnObj = ObjectManager::instance().InsertObject<Item>(_ObjectInfo.Position);
			std::dynamic_pointer_cast<Item>(_SpawnObj)->SetUpInitState(EItem::Explosive);
			SceneManageObjs.push_back(_SpawnObj);
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
			_SpawnObj = ObjectManager::instance().InsertObject<Item>(_ObjectInfo.Position);
			std::dynamic_pointer_cast<Item>(_SpawnObj)->SetUpInitState(EItem::Knife);
			SceneManageObjs.push_back(_SpawnObj);
			break;
		case ObjectInfo::Pomp:
			break;
		case ObjectInfo::Shotgun:
			break;
		case ObjectInfo::Smoke:
			_SpawnObj = ObjectManager::instance().InsertObject<Item>(_ObjectInfo.Position);
			std::dynamic_pointer_cast<Item>(_SpawnObj)->SetUpInitState(EItem::Smoke);
			SceneManageObjs.push_back(_SpawnObj);
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
		case ObjectInfo::Drum:
			_SpawnObj = ObjectManager::instance().InsertObject<Drum>(_ObjectInfo.Position);
			SceneManageObjs.push_back(_SpawnObj);
			break;
		case ObjectInfo::DoorWood:
			_SpawnObj = ObjectManager::instance().InsertObject<Door>(_ObjectInfo.Position);
			_Door = std::dynamic_pointer_cast<Door>(_SpawnObj);
			_Door->SetUpInitState(1, 0);
			SceneManageObjs.push_back(_SpawnObj);
			ObjectManager::instance()._Doors.push_back(_Door);
			break;
		case ObjectInfo::DoorPrison:
			_SpawnObj = ObjectManager::instance().InsertObject<Door>(_ObjectInfo.Position);
			_Door = std::dynamic_pointer_cast<Door>(_SpawnObj);
			_Door->SetUpInitState(2, 0);
			SceneManageObjs.push_back(_SpawnObj);
			ObjectManager::instance()._Doors.push_back(_Door);
			break;
		case ObjectInfo::DoorMansion:
			_SpawnObj = ObjectManager::instance().InsertObject<Door>(_ObjectInfo.Position);
			_Door = std::dynamic_pointer_cast<Door>(_SpawnObj);
			_Door->SetUpInitState(3, 0);
			SceneManageObjs.push_back(_SpawnObj);
			ObjectManager::instance()._Doors.push_back(_Door);
			break;
		case ObjectInfo::DoorClub:
			_SpawnObj = ObjectManager::instance().InsertObject<Door>(_ObjectInfo.Position);
			_Door = std::dynamic_pointer_cast<Door>(_SpawnObj);
			_Door->SetUpInitState(4, 0);
			SceneManageObjs.push_back(_SpawnObj);
			ObjectManager::instance()._Doors.push_back(_Door);
			break;
		default:
			break;
		}
	}
}
