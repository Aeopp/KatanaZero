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
	_Camera->CameraLockLT = { 798,1194,0 };
	_Camera->CameraLockRB = { 5357 ,2532 , 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());


	auto _Panic = ObjectManager::instance().InsertObject<PanicSwitch>(
		vec3{ 1060,2000,0 });

	_Panic->PushTrap({ 1100,2000,0 }, 1, vec3{ 1,0,0 }*100, 2.f);
	_Panic->PushTrap({ 1500,2000,0 }, 2, vec3{ -1,0,0 }*100, 2.f);
	_Panic->PushTrap({ 1300,2000,0 }, 3, vec3{ 1,0,0 }*200, 3.f);
	_Panic->PushTrap({ 1400,2000,0 }, 4, vec3{ 1,0,0 }*200, 3.f);
	_Panic->PushTrap({ 1500,2000,0 }, 5.f, {0,0,0}, 0.f);
	_Panic->PushTrap({ 1600,2000,0 }, 6.f, {0,0,0}, 0.f);

	SceneManageObjs.push_back(_Panic);

	/*for (int i = 0; i < 1; ++i)
	{
		auto spDoor = ObjectManager::instance().InsertObject<Door>({ 1300+(float)i*100,2048,0 });
		spDoor->SetUpInitState(i%4, 0);
		ObjectManager::instance()._Doors.push_back(spDoor);
		SceneManageObjs.push_back(spDoor);
	}*/
	
	Scene::Initialize();
}
