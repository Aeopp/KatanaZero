#include "stdafx.h"
#include "Time.h"
#include "GraphicDevice.h"
#include <iostream>
#include <sstream>
#include "MainApp.h"

void Time::Initialize(const uint32_t LimitFrame,std::chrono::milliseconds DeltaMax)
{
	using namespace std::chrono_literals;

	PrevTime = std::chrono::high_resolution_clock::now();
	Accumulator = 0ms;
	LimitDelta = 1000ms / (float)LimitFrame;
	this->DeltaMax = DeltaMax;
}

void  Time::Update()
{
	using namespace std::chrono_literals;

	std::chrono::time_point CurrentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::ratio<1, 1000>> Delta = CurrentTime - PrevTime;

	PrevTime = CurrentTime;
	Accumulator += Delta;
	SecCheck += Delta;

	App& AppRef = 	App::instance().instance();

	while (Accumulator >= LimitDelta)
	{
		DeltaTime = (LimitDelta.count() * 0.001f) * TimeScale;
		_T += DeltaTime;
		Accumulator -= LimitDelta;

		AppRef.Update();
		AppRef.LateUpdate();
	}

	if (SecCheck >= 1000ms)
	{
		FPS = _FPSCount;
		_FPSCount = 0;
		SecCheck = 0ms;
	}
	else
		++_FPSCount;

	AppRef.Render();
}

void Time::Render()
{
	RenderFPS();
}

float Time::Delta()
{
	return DeltaTime;
}

float Time::T()
{
	return _T;
}

void Time::SetDelta(const float DeltaTime)&
{
	this->DeltaTime = DeltaTime * TimeScale;
}

void Time::SetT(const float T)
{
	this->_T = T;
}

void Time::TimerRegist(float initial, float Repeat,float End, 
NotifyEventType _NotifyEvent)
{
	End = (std::max)(End, initial);
	_NotifyEvents.emplace_back(false,initial,Repeat,End,0.f,initial,std::move(_NotifyEvent));
}

void Time::NotificationCheck()&
{
	
	// 시작 시간 , 반복 주기 , 종료 시간 , 현재 측정 시간
	for(auto iter = std::begin(_NotifyEvents);
	iter!=std::end(_NotifyEvents);)
	{
		auto& [bInit,Init, Repeat, End, CurrentDelta, initial,Event] = *iter;
		
		Init -= DeltaTime;
		if (Init > 0.f)continue;
		
		if(Init<0.f && !bInit)
		{
			bInit = true;
			CurrentDelta = initial;
			continue;
		}
		
		CurrentDelta -= DeltaTime;
		End -= DeltaTime;
		bool IsNotifyEnd = false;
		
		if(CurrentDelta<0.f)
		{
			IsNotifyEnd |= Event();
			CurrentDelta = Repeat;
		}
		
		IsNotifyEnd |= (End < 0.f);
		
		if (IsNotifyEnd)
		{
			iter = _NotifyEvents.erase(iter);
			continue; 
		}
		
		++iter;
	}
}

void Time::RenderFPS() const& noexcept
{
	if (!bTimeInfoRender)return;

	std::wstringstream StrInfo;
	StrInfo << L"FPS : " << FPS << L"DeltaTime : " << DeltaTime << L"TimeScale : " << TimeScale <<
		L"플레이 시간 : " << _T;

	matrix MTranslation;
	D3DXMatrixTranslation(&MTranslation, 00.f, 300.f, 0.f);
	GraphicDevice::instance().GetSprite()->SetTransform(&MTranslation);
	GraphicDevice::instance().GetFont()->DrawTextW(GraphicDevice::instance().GetSprite(),
		StrInfo.str().c_str(), StrInfo.str().size(), nullptr, 0, D3DCOLOR_ARGB(255, 255, 255, 255));
}
