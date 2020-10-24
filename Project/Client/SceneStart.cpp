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

void SceneStart::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Prison";
	LoadMapTexInfoFilePath = L"..\\Data\\Prison\\3rd\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Prison\\3rd\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Prison\\3rd\\Line.txt"s;

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Player>
	(vec3{ 1180,5433,0.f }));

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	Scene::Initialize();
}
