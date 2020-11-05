#include "stdafx.h"
#include "SceneMultiPlayer.h"
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

void SceneMultiPlayer::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Multiplayer";
	LoadMapTexInfoFilePath = L"..\\Data\\Multiplayer\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Multiplayer\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Multiplayer\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Multiplayer\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Multiplayer\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EChinaTown1st;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 900,2000,0 };
	_Camera->CameraLockRB = { 3800 ,3300, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());


	Scene::Initialize();
}
