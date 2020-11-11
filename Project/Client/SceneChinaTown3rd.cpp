#include "stdafx.h"
#include "SceneChinaTown3rd.h"


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
//1408 2749

//4368 3726

void SceneChinaTown3rd::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"ChinaTown";
	LoadMapTexInfoFilePath = L"..\\Data\\ChinaTown\\3rd\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\ChinaTown\\3rd\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\ChinaTown\\3rd\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\ChinaTown\\3rd\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\ChinaTown\\3rd\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EChinaTown4th;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 1410,2650,0 };
	_Camera->CameraLockRB = { 4106,3747, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	Scene::Initialize();
}
