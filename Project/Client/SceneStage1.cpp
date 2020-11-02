#include "stdafx.h"
#include "SceneStage1.h"
#include "Texture_Manager.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Camera.h"
#include "Mouse.h"
#include "HUD.h"
#include "Battery.h"
#include "UITimer.h"
#include <string_view>

void SceneStage1::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"ChinaTown";
	LoadMapTexInfoFilePath = L"..\\Data\\ChinaTown\\1st\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\ChinaTown\\1st\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\ChinaTown\\1st\\Line.txt"s;
	// 테스트중인 수정바람
	LoadAStarInfoFilePath = L"..\\Data\\Prison\\3rd\\AStar.txt"s;
	// 	
	auto _Player =ObjectManager::instance().InsertObject<Player>
	(vec3{ 1309,3576,0.f });
	global::CameraPos = vec3{ 1309,3576,0.f };

	ObjectManager::instance()._Player = _Player;
	SceneManageObjs.push_back(_Player);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	Scene::Initialize();
}
