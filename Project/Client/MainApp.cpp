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

HRESULT App::Initialize()
{
#pragma region Application Initialize
	static constexpr bool bAppWindowScreenMode = TRUE;
	static constexpr uint32_t AppFrameLimit = 144ul;

	GraphicDevice& GraphicDeviceRef = GraphicDevice::instance();
	GraphicDeviceRef.Initialize(bAppWindowScreenMode);

	RenderManager& RenderManagerRef = RenderManager::instance();
	RenderManagerRef.Initialize();

	Time& TimeRef = Time::instance();
	TimeRef.Initialize(AppFrameLimit,global::DeltaMax);

	CollisionManager& CollisionManagerRef = CollisionManager::instance();
	CollisionManagerRef.Initialize();

	ObjectManager& ObjectManagerRef = ObjectManager::instance();
	ObjectManagerRef.Initialize();

	InputManager& InputManagerRef = InputManager::instance();
	InputManagerRef.Initialize();

	SceneManager& SceneMgrRef = SceneManager::instance();
	SceneMgrRef.Initialize();
	
	SceneMgrRef.Scene_Change(ESceneID::EStart);
#pragma endregion Application Initialize

	return S_OK; 
}

void App::Update()
{

	Time::instance().NotificationCheck();
	InputManager::instance().Update();
	ObjectManager::instance().Update();
	SceneManager::instance().Update();
	CollisionManager::instance().Update();
}

void App::LateUpdate()
{

	ObjectManager::instance().LateUpdate();

	SceneManager::instance().LateUpdate();

	CollisionManager::instance().LateUpdate();
}

void App::Render()
{

	GraphicDevice::instance().RenderBegin();

	{
		RenderManager::instance().Render();
		SceneManager::instance().Render();
		CollisionManager::instance().Render();
		Time::instance().Render();
	}

	GraphicDevice::instance().RenderEnd();
}

void App::Release()
{
	
}

