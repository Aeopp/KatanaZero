#pragma once
#include <Windows.h>
#include "Typedef.h"

class global
{
public:
	static constexpr std::pair<float, float > ClientSize = { 1600 ,950 };
	static constexpr std::pair<float, float > TileNums = { 100 ,100 };
	static constexpr std::pair<float, float > TileSize = { 36 ,36 };
	static constexpr int32_t ClientViewMinLeftTop = (int32_t(((ClientSize.first + ClientSize.second) / 3.7f) / 36)) *TileSize.first;
	static std::array<vec3, 4ul> GetScreenRect();
	
	static HWND hWND;
	static bool bDebug;
};

#define Sword	0x01		//0000 0001
#define Shoes		0x02		//0000 0010
#define Shocks		0x04		//0000 0100

#ifdef _AFX
#define  ERR_MSG(MSG) AfxMessageBox(MSG)
#else
#define ERR_MSG(MSG) MessageBox(nullptr, MSG, L"SystemError", MB_OK)
#endif // _AFX


