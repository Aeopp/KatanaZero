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
#include "Grunt.h"

void SceneStart::Initialize()
{
	using namespace std::string_view_literals;
	using namespace std::string_literals;

	CurrentMapStateKey = L"Prison";
	LoadMapTexInfoFilePath = L"..\\Data\\Prison\\3rd\\RenderTile.txt"s;
	LoadCollisionTileInfoFilePath = L"..\\Data\\Prison\\3rd\\Collision.txt"s;
	LoadLineInfoFilePath = L"..\\Data\\Prison\\3rd\\Line.txt"s;
	LoadAStarInfoFilePath = L"..\\Data\\Prison\\3rd\\AStar.txt"s;

	vec3 PlayerLocation = vec3{ 1180,5433,0.f };

	auto _Player =ObjectManager::instance().InsertObject<Player>
		(PlayerLocation);

	global::CameraPos = PlayerLocation;
	//global::CameraPos.x -= global::ClientSize.first / 2.f;
	//global::CameraPos.y -= global::ClientSize.second / 2.f;

	ObjectManager::instance()._Player = _Player;

	SceneManageObjs.push_back(_Player);
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<Mouse>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<HUD>());
	SceneManageObjs.push_back(ObjectManager::instance().InsertObject<UITimer>());

	auto _Grunt = ObjectManager::instance().InsertObject<Grunt>
		(vec3{ 3325,5365,0.f });
	_Grunt->SetUpInitState(-1.f, 0);

	SceneManageObjs.push_back(_Grunt);

	// TODO:: REMOVE PLZ TEST CODE 
	{
		float XDiff = 300.f;
		float x = 1876.f;
		float y = 5456;

		while (true)
		{
			auto _Grunt = ObjectManager::instance().InsertObject<Grunt>
				(vec3{ x,y,0.f });
			_Grunt->SetUpInitState(-1.f, 0);
			x += XDiff;
			if (x > 4300)
				break;
		}

		x = 3549;
		y = 5153;
		while (true)
		{
			auto _Grunt = ObjectManager::instance().InsertObject<Grunt>
				(vec3{ x,y,0.f });
			_Grunt->SetUpInitState(-1.f, 0);
			x += XDiff;
			if (x > 4863)
				break;
		}

		x = 3549;
		y = 5153 - 295;
		while (true)
		{
			auto _Grunt = ObjectManager::instance().InsertObject<Grunt>
				(vec3{ x,y,0.f });
			_Grunt->SetUpInitState(-1.f, 0);
			x += XDiff;
			if (x > 4863)
				break;
		}


	/*	x = 1645;
		y = 5153;
		while (true)
		{
			auto _Grunt = ObjectManager::instance().InsertObject<Grunt>
				(vec3{ x,y,0.f });
			_Grunt->SetUpInitState(-1.f, 0);
			x += XDiff;
			if (x > 4863)
				break;
		}

		x = 1645;
		y = 5153 - 295;
		while (true)
		{
			auto _Grunt = ObjectManager::instance().InsertObject<Grunt>
				(vec3{ x,y,0.f });
			_Grunt->SetUpInitState(-1.f, 0);
			x += XDiff;
			if (x > 4863)
				break;
		}*/

	}



	Scene::Initialize();
}
