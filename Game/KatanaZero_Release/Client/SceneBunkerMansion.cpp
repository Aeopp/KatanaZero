#include "stdafx.h"
#include "SceneBunkerMansion.h"

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
#include "Door.h"
#include "PanicSwitch.h"

void SceneBunkerMansion::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Bunkermansion";
	LoadMapTexInfoFilePath = L"..\\Data\\Bunkermansion\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Bunkermansion\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Bunkermansion\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Bunkermansion\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Bunkermansion\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EBoss;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 850,1160,0 };
	_Camera->CameraLockRB = { 5270,2500, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());


	auto _Panic = ObjectManager::instance().InsertObject<PanicSwitch>(
		vec3{ 2360,2358,0 });

	auto _PanicEnd = ObjectManager::instance().InsertObject<PanicSwitch>(
		vec3{ 3730,2354,0 });

	const float YScale = (2450.f - 2215)/213.f;
	const int32_t TrapNum = 13;
	const vec3 StartLocation = { 2500.f,(2450.f + 2215) / 2.f,0.f };
	const float XDiff = (3550.f - 2540.f) / TrapNum;

	for(size_t i=1;i< TrapNum+1;++i)
	{
		vec3 InitLocation = StartLocation;
		InitLocation.x += XDiff * i; 
		_PanicEnd->_Traps.push_back(_Panic->PushTrap(InitLocation, YScale, { 0,0,0 }, 0));
	}

	SceneManageObjs.push_back(_Panic);
	SceneManageObjs.push_back(_PanicEnd);
	
	Scene::Initialize();
}
