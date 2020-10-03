#pragma once
#include "singleton_interface.h"
#include <d3d9.h>
#include <d3dx9.h>

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
	// ��ġ�� �����ϰ� �������ִ� COM Obj. 
	LPDIRECT3D9 pSDK; 
	// ����̽� ��Ʈ�� COM Obj
	LPDIRECT3DDEVICE9 pDevice; 
	LPD3DXSPRITE pSprite; 
	LPD3DXFONT pFont; 
	LPD3DXLINE pLine; 
// component object model
};

