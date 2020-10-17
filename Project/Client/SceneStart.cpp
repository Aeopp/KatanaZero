#include "stdafx.h"
#include "SceneStart.h"
#include "Texture_Manager.h"
#include "ObjectManager.h"

#include <string_view>
#include "Camera.h"

void SceneStart::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	LoadTextureFilePath = L"..\\Data\\PathInfo.txt"s;
	LoadMapTexInfoFilePath = L"..\\Data\\Bunkermansion\\BunkermansionRenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Bunkermansion\\BunkermansionCollision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Bunkermansion\\BunkermansionLine.txt"s;
	
	manage_objs.push_back(ObjectManager::instance().InsertObject<Camera>());

	Scene::Initialize();
}
