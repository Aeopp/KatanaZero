#include "stdafx.h"
#include "Record.h"
#include "RecordManager.h"
#include "Time.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include <optional>

 void Record::Render()
 {
	 if (_Infos.empty())return;

	 auto _Inforange= _Infos.equal_range(RecordManager::instance().Timing);

	 for (; _Inforange.first != _Inforange.second; ++_Inforange.first)
	 {
		 Info& _Info = _Inforange.first->second;

		 auto TexInfo = TextureManager::instance().Get_TexInfo(_Info.ObjKey, _Info.StateKey, _Info.DrawID);
		 RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width  * _Info.SrcScale.x,
					   TexInfo->ImageInfo.Height * _Info.SrcScale.y };
		 vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
		 GraphicDevice::instance().GetSprite()->SetTransform(&_Info.MWorld);
		 GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
			 &_srcRT, &__TextureCenter, nullptr,
			 _Info._Color);
	 }
 }
