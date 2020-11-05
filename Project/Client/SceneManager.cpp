#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneBoss.h"
#include "ObjectManager.h"
#include "ComponentManager.h"
#include "Texture_Manager.h"
#include "EffectManager.h"

#include "ScenePrison1st.h"
#include "ScenePrison2nd.h"
#include "ScenePrison3rd.h"
#include "SceneBoss.h"
#include "SceneBunkerMansion.h"
#include "SceneMultiPlayer.h"
#include "SceneChinaTown1st.h"
#include "SceneChinaTown2nd.h"
#include "SceneChinaTown3rd.h"
#include "SceneChinaTown4th.h"

 void SceneManager::Scene_Change(ESceneID _ESceneID)
{
	_CurScene = _ESceneID;

	ObjectManager::instance().Clear();
	ComponentManager::instance().Clear();
	EffectManager::instance().Clear();

	/*if (_PreScene != _CurScene)
	{*/
		if (sp_scene)
		{
			sp_scene.reset();
		}
		sp_scene = MakeSceneTypeMap[_ESceneID]();

		sp_scene->Initialize();
		_PreScene = _CurScene;
	//}
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
	SceneRegist<ESceneID::EPrison3rd, ScenePrison3rd>();
	SceneRegist<ESceneID::EBoss, SceneBoss>();
	SceneRegist<ESceneID::EPrison2nd, ScenePrison2nd>();
	SceneRegist<ESceneID::EPrison1st,ScenePrison1st>();
	SceneRegist<ESceneID::EMultiPlayer, SceneMultiPlayer>();
	SceneRegist<ESceneID::EBunkerMansion,SceneBunkerMansion>();
	SceneRegist<ESceneID::EChinaTown1st, SceneChinaTown1st>();
	SceneRegist<ESceneID::EChinaTown2nd, SceneChinaTown2nd>();
	SceneRegist<ESceneID::EChinaTown3rd, SceneChinaTown3rd>();
	SceneRegist<ESceneID::EChinaTown4th, SceneChinaTown4th>();
}

void SceneManager::Release()
{
	if (!sp_scene)return;
	sp_scene->Release();
}


