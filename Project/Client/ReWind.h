#pragma once

class ReWind
{
public:
	struct Info
	{
		// 되감기 도중 알파가 0이 되면 삭제
		float OwnerY = 0.f;
		float Alpha = 255;
		float T = 0.f;
		matrix MWorld;
		std::wstring_view ObjKey;
		std::wstring_view StateKey;
		uint8_t DrawID;
	};
public:
	// 되감기 중이 아닐때만 호출
	void Push(float T, matrix MWorld, std::wstring_view ObjKey, std::wstring_view StateKey,
		uint8_t DrawID, float OwnerY);
	// 매틱마다 호출해주세요.
	void Update();
	void Render();
	bool bUpdate = false;
private:
	
	float UpdateDelta = 0.1f;
	std::list<typename ReWind::Info> _Infos;
};
