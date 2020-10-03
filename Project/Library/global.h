#pragma once
#include <utility>
#include <Windows.h>

class global
{
public:
	static constexpr std::pair<float, float > ClientSize = { 1920 ,1080 };
	static constexpr std::pair<float, float > TileNums = { 100 ,100 };
	static constexpr std::pair<float, float > TileSize = { 32 ,32 };
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


