#include "stdafx.h"
#include "ScenePrison2nd.h"
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


void ScenePrison2nd::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Prison";
	LoadMapTexInfoFilePath = L"..\\Data\\Prison\\2nd\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Prison\\2nd\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Prison\\2nd\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Prison\\2nd\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Prison\\2nd\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EPrison3rd;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 792,976,0 };
	_Camera->CameraLockRB = { 6184,2475, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());


	Scene::Initialize();

}
