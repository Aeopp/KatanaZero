#pragma once
#include "singleton_interface.h"

class GraphicDevice final : public singleton_interface <GraphicDevice>
{
public:
	virtual ~GraphicDevice()noexcept;
public:
	LPDIRECT3DDEVICE9 GetDevice() { return pDevice; }
	LPD3DXSPRITE GetSprite() { return pSprite; }
	LPD3DXFONT GetFont() { return pFont; }
	LPD3DXLINE GetLine() { return pLine;  }
public:
	HRESULT Ready();
	void Release();
	void RenderBegin();
	void RenderEnd(HWND hWND = nullptr);
private:
	// 장치를 조사하고 생성해주는 COM Obj. 
	LPDIRECT3D9 pSDK; 
	// 디바이스 컨트롤 COM Obj
	LPDIRECT3DDEVICE9 pDevice; 
	LPD3DXSPRITE pSprite; 
	LPD3DXFONT pFont; 
	LPD3DXLINE pLine; 
// component object model
};

