#include "stdafx.h"
#include "ReWind.h"
#include "RecordManager.h"
#include "Time.h"

void ReWind::Push(float T, matrix MWorld, std::wstring_view ObjKey, std::wstring_view StateKey, uint8_t DrawID, float OwnerY)
{
	if (RecordManager::instance().bReWind) return;

	Info _Info;
	_Info.T = T;
	_Info.MWorld = std::move(MWorld);
	_Info.ObjKey = std::move(ObjKey);
	_Info.StateKey = std::move(StateKey);
	_Info.DrawID = DrawID;
	_Info.OwnerY = OwnerY;

	_Infos.emplace_back(std::move(_Info));
};

 void ReWind::Update()
 {
	 if (RecordManager::instance().bReWind)
	 {
		 const float dt = Time::instance().Delta();
		 UpdateDelta -= dt;
		 if (UpdateDelta < 0.f)
		 {
			 UpdateDelta = 0.1f;
			 bUpdate = true;
			 return;
		 }
		 else
		 {
			 bUpdate = false;
		 }
	}
 }

 void ReWind::Render()
 {
	 const float dt = Time::instance().Delta();

	 /*auto RIter = std::rbegin(_Infos);
	 while (RIter != std::rend(_Infos))
	 {
		 if (RIter->Alpha >= 255)
		 {
			 break;
		 }
		 else
		 {
			 ++RIter;
		 }
	 }

	 auto Iter = RIter.base();
	 for (; Iter != std::end(_Infos); )
	 {

	 }*/
	 auto RIter = std::rbegin(_Infos);

	 std::find_if(std::rbegin(RIter),)
 }
