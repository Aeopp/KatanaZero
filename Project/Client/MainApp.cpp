#include "stdafx.h"
#include "MainApp.h"
#include "Player.h"
#include "GraphicDevice.h"
#include "SceneManager.h"
#include "Time.h"
#include "RenderManager.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "InputManager.h"
#include "ComponentManager.h"
#include "Texture_Manager.h"
#include "AStarManager.h"
#include "EffectManager.h"
#include "RecordManager.h"


HRESULT App::Initialize()
{
	ShowCursor(false);

#pragma region Application Initialize
	static constexpr bool bAppWindowScreenMode = TRUE;
	static constexpr uint32_t AppFrameLimit = 60ul;

	GraphicDevice& GraphicDeviceRef = GraphicDevice::instance();
	GraphicDeviceRef.Initialize(bAppWindowScreenMode);

	InputManager& InputManagerRef = InputManager::instance();
	InputManagerRef.Initialize();

	RenderManager& RenderManagerRef = RenderManager::instance();
	RenderManagerRef.Initialize();

	Time& TimeRef = Time::instance();
	TimeRef.Initialize(AppFrameLimit,global::DeltaMax);

	ComponentManager::instance().Initialize();

	CollisionManager& CollisionManagerRef = CollisionManager::instance();
	CollisionManagerRef.Initialize();

	ObjectManager& ObjectManagerRef = ObjectManager::instance();
	ObjectManagerRef.Initialize();

	EffectManager::instance().Initialize();

	TextureManager::instance().LoadTexturesFromTexInfoFile(L"..\\Data\\PathInfo.txt"s);

	SceneManager& SceneMgrRef = SceneManager::instance();
	SceneMgrRef.Initialize();
	
	SceneMgrRef.Scene_Change(ESceneID::EBoss);

	//TimeRef.TimerRegist(10.f, 10.f, 10.f, [&]()
	//{
	//	TimeRef.SetT(0.f);
	//	global::bRePlay = true;
	//	SceneMgrRef.Scene_Change(ESceneID::EStart);
	//	return true;
	//});
	//
	//TimeRef.TimerRegist(21.f, 21.f, 21.f, [&]()
	//{
	//	TimeRef.SetT(0.f);
	//	global::bRePlay = false;
	//	SceneMgrRef.Scene_Change(ESceneID::EStart);
	//	return true;
	//});

#pragma endregion Application Initialize

	return S_OK; 
}

void App::Update()
{
	Time::instance().NotificationCheck();
	InputManager::instance().Update();
	ObjectManager::instance().Update();
	ComponentManager::instance().Update();
	CollisionManager::instance().Update();
	EffectManager::instance().Update();
	SceneManager::instance().Update();
}

void App::LateUpdate()
{
	ObjectManager::instance().LateUpdate();
	ComponentManager::instance().LateUpdate();
	CollisionManager::instance().LateUpdate();
	SceneManager::instance().LateUpdate();
}

void App::Render()
{
	RecordManager::instance().Update();

	GraphicDevice::instance().RenderBegin();

	{
		ComponentManager::instance().Render();
		RenderManager::instance().Render();
		
		CollisionManager::instance().Render();
		AStarManager::instance().DebugRender();
		SceneManager::instance().Render();
		Time::instance().Render();
	}

	GraphicDevice::instance().RenderEnd();
}

void App::Release()
{
	
}

