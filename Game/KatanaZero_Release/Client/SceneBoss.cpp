#include "stdafx.h"
#include "SceneBoss.h"

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
#include "Grunt.h"
#include "Item.h"
#include "Drum.h"
#include "SmokeCloud.h"
#include "Boss.h"

// 
//
// 

void SceneBoss::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Multiplayer";
	LoadMapTexInfoFilePath = L"..\\Data\\Boss\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Boss\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Boss\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Boss\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Boss\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EMultiPlayer;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 1047,903,0 };
	_Camera->CameraLockRB = { 2905,1929, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	Scene::Initialize();

	auto _Boss =ObjectManager::instance().InsertObject<Boss>({ 2650,1697,0 });
	SceneManageObjs.push_back(_Boss);
	ObjectManager::instance()._Boss = _Boss;
	// 2468 1697 
}
