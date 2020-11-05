#include "stdafx.h"
#include "SceneChinaTown1st.h"

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

void SceneChinaTown1st::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"ChinaTown";
	LoadMapTexInfoFilePath = L"..\\Data\\ChinaTown\\1st\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\ChinaTown\\1st\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\ChinaTown\\1st\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\ChinaTown\\1st\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\ChinaTown\\1st\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EChinaTown2nd;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 1116,2951,0 };
	_Camera->CameraLockRB = { 4235,3786, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());


	Scene::Initialize();
}
