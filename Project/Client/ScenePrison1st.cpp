#include "stdafx.h"
#include "ScenePrison1st.h"

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
#include "LaserTrap.h"

void ScenePrison1st::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Prison";
	LoadMapTexInfoFilePath = L"..\\Data\\Prison\\1st\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Prison\\1st\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Prison\\1st\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Prison\\1st\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Prison\\1st\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EPrison2nd;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 2922,4680,0 };
	_Camera->CameraLockRB = { 6002,5931, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());


	
	
	{
		const vec3 InitLocation = { 4717,5728.5f,0.f };
		const float YScale = 1.309859;
		const float XDiff = 73;
		for (int32_t i = 0; i < 6; ++i)
		{
			auto _LaserTrap = ObjectManager::instance().InsertObject<LaserTrap>();
			vec3 SpawnLocation = InitLocation;
			SpawnLocation.x += XDiff * i;
			_LaserTrap->Init(std::move(SpawnLocation), YScale, { 0,0,0 }, 0);
			SceneManageObjs.push_back(_LaserTrap);
		}
	}

	{
		const vec3 InitLocation = { 5578,(5508 + 5157) / 2.f,0.f };
		const float XDiff = 5800 - 5578;
		const float YScale = 1.647887;
		for (int32_t i = 0; i < 2; ++i)
		{
			auto _LaserTrap = ObjectManager::instance().InsertObject<LaserTrap>();
			vec3 SpawnLocation = InitLocation;
			SpawnLocation.x += XDiff * i;
			_LaserTrap->Init(std::move(SpawnLocation), YScale, { 0,0,0 }, 0);
			SceneManageObjs.push_back(_LaserTrap);
		}
	}

	{
		const float YScale = (5078.f - 4797.f) / 213.f;
		const float XDiff = 80;
		const vec3 InitLocation = { 5133,(5078 + 4797) / 2.f,0.f };
		const vec3 InitDir = { 1,0,0 };
		const float Speed = 200.f;
		for (int32_t i = 0; i < 3; ++i)
		{
			auto _LaserTrap = ObjectManager::instance().InsertObject<LaserTrap>();
			vec3 SpawnLocation = InitLocation;
			SpawnLocation.x += XDiff * i;
			_LaserTrap->Init(std::move(SpawnLocation), YScale, InitDir*Speed, 3.f);
			SceneManageObjs.push_back(_LaserTrap);
		}
	}


	Scene::Initialize();
}
