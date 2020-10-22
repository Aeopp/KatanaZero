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

	LoadTextureFilePath = L"..\\Data\\PathInfo.txt"s;
	LoadMapTexInfoFilePath = L"..\\Data\\Bunkermansion\\BunkermansionRenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Bunkermansion\\BunkermansionCollision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Bunkermansion\\BunkermansionLine.txt"s;

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Player>(vec3{1000,2000,0.f}));
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	Scene::Initialize();
}
