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
#include "Texture_Manager.h"
#include "Camera.h"
#include "ObjectManager.h"

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

	/*_Anys.emplace_back(InputManager::instance().EventRegist([this]()
	{ if (global::IsReWind() && bRewindEnd) { RecordManager::instance().bRewindEnd = false; ReWindEnd(); };   },
		VK_LBUTTON, InputManager::EKEY_STATE::DOWN));*/
}

void RecordManager::Update()
{
	if (bUpdate)
	{
		_TimingCameraPos[Timing] = global::CameraPos;
		++Timing;
	}
};

void RecordManager::ReWindUpdate()
{
	InputManager::instance().Update();

	Timing += RewindSpeed;
	if (Timing <= 1)
	{
		Timing = 1;
		//bRewindEnd = true;
		ReWindEnd();
	}
	// TODO :: ���⼭ ���� �Ŵ��� ȣ�� ������� ������
	global::CameraPos = _TimingCameraPos[Timing];

};

void RecordManager::ReWindStart()
{
	global::_CurGameState = global::ECurGameState::ReWind;
	RewindSpeed =  -(Timing / 100);
	if (RewindSpeed >= -1)
	{
		RewindSpeed = -1;
	}
	//	InputManager::instance().Clear();
};

void RecordManager::ReWindEnd()
{
	global::_CurGameState = global::ECurGameState::Play;

	ObjectManager::instance().bEnemyUpdate = true;

	EffectManager::instance().Clear();
	EffectManager::instance().RecordClear();
	//	global::CameraPos = _TimingCameraPos[Timing];

	_TimingCameraPos.clear();
	if (!_TimingCameraPos.empty())
	{
		//auto LastCameraPos = _TimingCameraPos.end();
		//std::advance(LastCameraPos, -100);
		//global::CameraPos= LastCameraPos->second;
		_TimingCameraPos.clear();
	};

	ObjectManager::instance()._Camera.lock()->bUpdate = true;
	Time::instance()._T = 0;

	RewindSpeed = -8;
	TimingSpeed = 1;
	EndTiming = 0;
	bPause = false;
	bReplayInit = false;
	Timing = 0;
	bUpdate = false;


	SceneManager::instance().Scene_Change(SceneManager::instance().GetCurrentSceneID());
};

void RecordManager::ReplayStart()
{
	EffectManager::instance().Clear();
	RenderManager::instance().bUIRender = false;
	ObjectManager::instance()._Camera.lock()->bUpdate = false;
	bUpdate = false;

	vec3 ScreenCenter = global::CameraPos;
	ScreenCenter.x += global::ClientSize.first / 2.f;
	ScreenCenter.y += global::ClientSize.second / 2.f;

	//EffectManager::instance().EffectPush(L"Effect", L"YouCanDoThis",
	//	2, 2, 2, OBJECT_ID::EID::ENONE, false, ScreenCenter, { 0,0,0 }, { 1,1,1 });

	Time::instance().TimerRegist(2.f, 2.f, 2.f, []() {
		RenderManager::instance().bUIRender = true;
		ObjectManager::instance()._Camera.lock()->bUpdate = true;
		RecordManager::instance().bReplayInit = true;
		return true;
	});

	EndTiming = Timing;
	Timing = 0;
	global::_CurGameState = global::ECurGameState::Replay;
};

void RecordManager::ReplayEnd()
{
	global::_CurGameState = global::ECurGameState::Play;

	EffectManager::instance().Clear();
	EffectManager::instance().RecordClear();
	_TimingCameraPos.clear();
	if (!_TimingCameraPos.empty())
	{
		/*auto LastCameraPos = _TimingCameraPos.end();
		std::advance(LastCameraPos, -1);
		global::CameraPos = LastCameraPos->second;*/
		_TimingCameraPos.clear();
	}
	bReplayInit = false;
	Time::instance()._T = 0;
	RecordManager::Timing = 0;

	SceneManager::instance().Scene_Change(SceneManager::instance().GetNextSceneID());

	int i = 0;

}
void RecordManager::ReplayUpdate()
{
	if (!bReplayInit)
	{
		InputManager::instance().Update();
		Time::instance().NotificationCheck();
		return;
	};

	InputManager::instance().Update();
	if (_TimingCameraPos.empty())
	{
		if(global::IsReplay())
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

	if (Timing <= 1)
	{
		Timing = 1;
	}
}
void RecordManager::RePlayRender()
{
	GraphicDevice::instance().RenderBegin();

	if (!bReplayInit)
	{
		auto TexInfo = TextureManager::instance().Get_TexInfo(L"Effect", L"YouCanDoThis", 0);
		RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width ,
					  TexInfo->ImageInfo.Height };
		vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
		matrix MScale, MTrans, MWorld;
		D3DXMatrixScaling(&MScale, 1, 1, 0);
		D3DXMatrixTranslation(&MTrans, 1920.f / 2.f, 1080.f / 2.f, 0);
		MWorld = MScale * MTrans;
		GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
		GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
			&_srcRT, &__TextureCenter, nullptr,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		RenderManager::instance()._Terrain.Render();

		auto& RenderCompVec = ComponentManager::instance().Find<RenderComponent>();

		for (auto& _RenderComp : RenderCompVec)
		{
			_RenderComp->RecordRender();
		}

		EffectManager::instance().RecordRender();
		float diff = EndTiming - Timing;
		diff /= EndTiming;

		{
			int32_t Alpha = 160 - (diff * 160);

			D3DXCOLOR _Color = D3DCOLOR_ARGB(Alpha, 255, 255, 255);

			auto TexInfo = TextureManager::instance().Get_TexInfo(L"Effect", L"ReplayBack", 0);
			RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width ,
						  TexInfo->ImageInfo.Height };
			vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
			matrix MScale, MTrans, MWorld;
			D3DXMatrixScaling(&MScale, 1920, 1080, 0);
			D3DXMatrixTranslation(&MTrans, 1920.f / 2.f, 1080.f / 2.f, 0);
			MWorld = MScale * MTrans;
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
				&_srcRT, &__TextureCenter, nullptr,
				_Color);
		}

		{
			auto TexInfo = TextureManager::instance().Get_TexInfo(L"MYUI",
				L"Replay", 0);
			RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width,
			TexInfo->ImageInfo.Height };
			vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
			matrix MWorld;
			D3DXMatrixTranslation(&MWorld, 1800, 850, 0);
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
				&_srcRT, &__TextureCenter, nullptr,
				D3DCOLOR_ARGB(255,255,255,255));
		}

		int32_t DrawID = TimingSpeed < 0 ? 2 : 1;
		DrawID = TimingSpeed == 0 ? 3 : DrawID;

		{
			auto TexInfo = TextureManager::instance().Get_TexInfo(L"MYUI",
				L"Replay", DrawID);
			RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width,
			TexInfo->ImageInfo.Height };
			vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
			matrix MWorld;
			D3DXMatrixTranslation(&MWorld, 200, 100, 0);
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
				&_srcRT, &__TextureCenter, nullptr,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
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