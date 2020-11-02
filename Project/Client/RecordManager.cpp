#include "stdafx.h"
#include "RecordManager.h"
#include "Time.h"

void RecordManager::TimeCameraPosPush(vec3 CameraPos)
{
	_TimeCameraPos.emplace_back(std::pair<float, vec3>{
		Time::instance().T(), CameraPos});
};

vec3 RecordManager::GetCameraPos()
{
	vec3 CameraPos{ 0,0,0 };

	while (!_TimeCameraPos.empty())
	{
		if (_TimeCameraPos.back().first < ReWindT)
		{
			CameraPos = std::move(_TimeCameraPos.back().second);
			_TimeCameraPos.pop_back();
			return CameraPos;
		}
		else
		{
			_TimeCameraPos.pop_back();
		}
	}

	return CameraPos;
};

void RecordManager::DeathRecord()
{
	//if (global::_CurGameState == global::ECurGameState::Play || 
	//	global::_CurGameState == global::ECurGameState::Slow)
	{
		ReWindT = AtDeathT = Time::instance().T();
	}
};

void RecordManager::Update()
{
	//if (global::_CurGameState == global::ECurGameState::ReWind)
	{
		const float dt = Time::instance().Delta();
		
		ReWindT -= dt;
	}
}
void RecordManager::ReWindStart()
{
	bReWind = true;
};
