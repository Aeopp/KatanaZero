#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneStart.h"
#include "SceneStage1.h"
#include "SceneStage2.h"
#include "SceneBoss.h""

 void SceneManager::Scene_Change(ESceneID _ESceneID)
{
	_CurScene = _ESceneID;
	if (_PreScene != _CurScene)
	{
		if (sp_scene)
		{
			sp_scene.reset();
		}
		sp_scene = MakeSceneTypeMap[_ESceneID]();

		sp_scene->Initialize();
		_PreScene = _CurScene;
	}
}

void SceneManager::Render()
{
	if (!sp_scene)return;
	sp_scene->Render();
}

void SceneManager::Update()
{
	if (!sp_scene)return;
	sp_scene->Update();
}

void SceneManager::LateUpdate()
{
	if (!sp_scene)return;
	sp_scene->LateUpdate();
}

void SceneManager::Initialize()
{
	//Scene µî·Ï
	SceneRegist<ESceneID::EStart, SceneStart>();
	SceneRegist<ESceneID::EStage1, SceneStage1>();
	SceneRegist<ESceneID::EStage2, SceneStage2>();
	SceneRegist<ESceneID::EBoss, SceneBoss>();
}

void SceneManager::Release()
{
	if (!sp_scene)return;
	sp_scene->Release();
}


