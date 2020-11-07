#include "stdafx.h"
#include "SceneBunkerMansion.h"

#include "Texture_Manager.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Camera.h"
#include "Mouse.h"
#include "HUD.h"
#include "Battery.h"
#include "UITimer.h"
#include <string_view>
#include "Grunt.h"
#include "SceneManager.h"
#include "Gangster.h"
#include "GO.h"
#include "Door.h"


void SceneBunkerMansion::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Bunkermansion";
	LoadMapTexInfoFilePath = L"..\\Data\\Bunkermansion\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Bunkermansion\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Bunkermansion\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Bunkermansion\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Bunkermansion\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EBoss;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 798,1194,0 };
	_Camera->CameraLockRB = { 5357 ,2532 , 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	/*for (int i = 0; i < 1; ++i)
	{
		auto spDoor = ObjectManager::instance().InsertObject<Door>({ 1300+(float)i*100,2048,0 });
		spDoor->SetUpInitState(i%4, 0);
		ObjectManager::instance()._Doors.push_back(spDoor);
		SceneManageObjs.push_back(spDoor);
	}*/
	
	Scene::Initialize();
}
