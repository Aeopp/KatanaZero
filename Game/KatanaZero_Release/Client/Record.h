#pragma once
#include <optional>

class Record
{
public:
	struct Info
	{
		// 되감기 도중 알파가 0이 되면 삭제
		float OwnerY = 0.f;
		float Alpha = 255;
		int32_t Timing;
		matrix MWorld;
		std::wstring ObjKey;
		std::wstring StateKey;
		uint8_t DrawID;
		D3DXCOLOR _Color;
		vec2 SrcScale{ 1,1 };
	};
public:
	// 매틱마다 호출해주세요.
	void Render();
	std::multimap<int32_t, typename Record::Info> _Infos;
};
