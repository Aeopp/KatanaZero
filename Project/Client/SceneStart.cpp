#include "stdafx.h"
#include "SceneStart.h"
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


void SceneStart::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Prison";
	LoadMapTexInfoFilePath = L"..\\Data\\Prison\\3rd\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Prison\\3rd\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Prison\\3rd\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Prison\\3rd\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Prison\\3rd\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EStage1;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT= { 700,4400,0 };
	_Camera->CameraLockRB = { 5000 ,5600 , 0  };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());



	Scene::Initialize();
}
