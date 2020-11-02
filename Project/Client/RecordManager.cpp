#include "stdafx.h"
#include "RecordManager.h"
#include "Time.h"
#include "RenderManager.h"
#include "EffectManager.h"
#include "ComponentManager.h"
#include "Component.h"
#include "RenderComponent.h"
#include "InputManager.h"
#include "GraphicDevice.h"
#include "SceneManager.h"

RecordManager::RecordManager()
{
	_Anys.emplace_back(InputManager::instance().EventRegist([this]()
	{ if (global::IsReplay())	++TimingSpeed; },
		VK_RIGHT, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this]()
	{ if (global::IsReplay())--TimingSpeed; },
		VK_LEFT, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this]()
	{ if (global::IsReplay())	TimingSpeed = 0; },
		VK_SPACE, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this]()
	{ if (global::IsReplay())   ReplayEnd();   },
		VK_BACK, InputManager::EKEY_STATE::DOWN));
}

void RecordManager::Update()
{
	_TimingCameraPos[Timing] = global::CameraPos;
	++Timing;
};

void RecordManager::ReWindUpdate()
{
	InputManager::instance().Update();

	if (Timing < 0)
	{
		ReWindEnd();
	}
	// TODO :: 여기서 사운드 매니저 호출 배경음만 나오게
	global::CameraPos = _TimingCameraPos[Timing];
	Timing += RewindSpeed;
};

void RecordManager::ReWindStart()
{
	global::_CurGameState = global::ECurGameState::ReWind;
	//	InputManager::instance().Clear();
};

void RecordManager::ReWindEnd()
{
	global::_CurGameState = global::ECurGameState::Play;

	EffectManager::instance().Clear();

	_TimingCameraPos.clear();
	Time::instance()._T = 0;
	RecordManager::Timing = 0;

	SceneManager::instance().Scene_Change(SceneManager::instance().GetCurrentSceneID());
};

void RecordManager::ReplayStart(ESceneID _SceneID)
{
	global::_CurGameState = global::ECurGameState::Replay;

	_AtReplayEndChangeSceneID = _SceneID; 

	RecordManager::Timing = 0;
};

void RecordManager::ReplayEnd( )
{
	global::_CurGameState = global::ECurGameState::Play;

	EffectManager::instance().Clear();

	_TimingCameraPos.clear();
	Time::instance()._T = 0;
	RecordManager::Timing = 0;

	SceneManager::instance().Scene_Change(_AtReplayEndChangeSceneID);
}
void RecordManager::ReplayUpdate()
{
	InputManager::instance().Update();
	if (_TimingCameraPos.empty())
	{
		ReplayEnd();
	}
	auto IsTiming = _TimingCameraPos.find(Timing);
	if (IsTiming == end(_TimingCameraPos))
	{
		std::advance(IsTiming, -1);
		Timing = IsTiming->first;
	}
	global::CameraPos = IsTiming->second;
	Timing += TimingSpeed;

	if (Timing < 0)
	{
		Timing = 0;
	}
}
void RecordManager::RePlayRender()
{
	GraphicDevice::instance().RenderBegin();

	RenderManager::instance()._Terrain.Render();

	auto& RenderCompVec = ComponentManager::instance().Find<RenderComponent>();

	for (auto& _RenderComp : RenderCompVec)
	{
		_RenderComp->RecordRender();
	}

	EffectManager::instance().RecordRender();

	Time::instance().Render();

	GraphicDevice::instance().RenderEnd();
};

void RecordManager::ReWindRender()
{
	GraphicDevice::instance().RenderBegin();

	RenderManager::instance()._Terrain.Render();

	auto& RenderCompVec = ComponentManager::instance().Find<RenderComponent>();

	for (auto& _RenderComp : RenderCompVec)
	{
		_RenderComp->RecordRender();
	}

	EffectManager::instance().RecordRender();

	Time::instance().Render();

	GraphicDevice::instance().RenderEnd();
};
