#include "stdafx.h"
#include "SceneBoss.h"

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
#include "Grunt.h"
#include "Item.h"
#include "Drum.h"
#include "SmokeCloud.h"


// 
//
// 

void SceneBoss::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Multiplayer";
	LoadMapTexInfoFilePath = L"..\\Data\\Boss\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Boss\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Boss\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Boss\\AStar.txt"s;
	LoadObjectInfoFilePath = L"..\\Data\\Boss\\ObjectInfo.txt"s;

	SceneManager::instance()._NextScene = ESceneID::EMultiPlayer;
	auto _Camera = ObjectManager::instance().InsertObject<Camera>();
	_Camera->CameraLockLT = { 1047,903,0 };
	_Camera->CameraLockRB = { 2905,1929, 0 };
	ObjectManager::instance()._Camera = _Camera;
	SceneManageObjs.push_back(_Camera);

	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	Scene::Initialize();

	//auto _Grunt = ObjectManager::instance().InsertObject<Grunt>(vec3{ 2100,1400 ,0 });
	//SceneManageObjs.push_back(_Grunt);

	// // _Grunt->_TransformComp->Position = ;

	/////////// TESTCODE 
	{
		auto _Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Knife);
		_Item->_TransformComp->Position = vec3{ 1500,1800,0 };
		SceneManageObjs.push_back(_Item);

		_Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Explosive);
		_Item->_TransformComp->Position = vec3{ 1700, 1800,0 };
		SceneManageObjs.push_back(_Item);

		_Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Smoke);
		_Item->_TransformComp->Position = vec3{ 1900, 1800,0 };
		SceneManageObjs.push_back(_Item);

		_Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Smoke);
		_Item->_TransformComp->Position = vec3{ 2000, 1800,0 };
		SceneManageObjs.push_back(_Item);

		_Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Smoke);
		_Item->_TransformComp->Position = vec3{ 2100, 1800,0 };
		SceneManageObjs.push_back(_Item);

		_Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Smoke);
		_Item->_TransformComp->Position = vec3{ 1800, 1800,0 };
		SceneManageObjs.push_back(_Item);

		_Item = ObjectManager::instance().InsertObject<Item>();
		_Item->SetUpInitState(EItem::Smoke);
		_Item->_TransformComp->Position = vec3{ 1700, 1800,0 };
		SceneManageObjs.push_back(_Item);


		vec3 InitLocation = { 1500, 1800, 0 };

		for (int i = 0; i < 12; ++i)
		{
			auto _Drum = ObjectManager::instance().InsertObject<Drum>();
			_Drum->_TransformComp->Position = InitLocation;
			SceneManageObjs.push_back(_Drum);
			InitLocation.x += 100;

			auto _Smoke = ObjectManager::instance().InsertObject<SmokeCloud>(InitLocation);
			SceneManageObjs.push_back(_Smoke);
		}	

		
	}
	///////////
}
