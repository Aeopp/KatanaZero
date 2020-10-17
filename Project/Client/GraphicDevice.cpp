#include "stdafx.h"
#include "GraphicDevice.h"

GraphicDevice::~GraphicDevice()
{
	Release();
}

HRESULT GraphicDevice::Initialize(const bool bFullScreen)
{
	// 장치를 생성하는 과정. 
	// 1.장치를 조사할 컴객체(m_pSDK) 생성. 
	// 2. 장치의 수준 조사, 
	// 3. pSDK를 이용하여 장치를 제어하는 컴객체(m_pDevice)를 생성.

	D3DCAPS9 DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	// 현재 다이렉트 버전에 맞는 컴객체를 생성

	//D3DDEVTYPE_HAL 
	if (FAILED(pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		ERR_MSG(L"Device Caps Load Failed");
		return E_FAIL;
	}


	DWORD vp = 0;
	// VERTEXPROCESSING
	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = global::ClientSize.first;
	d3dpp.BackBufferHeight = global::ClientSize.second; // 백버퍼 가로세로 크기 지정. 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = global::hWND;
	d3dpp.Windowed = bFullScreen;// 창모드일 경우 true, 전체화면일 경우 false, 
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	if (FAILED(pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		global::hWND, vp, &d3dpp, &pDevice)))
	{
		ERR_MSG(L"Device Creating Failed!");
		return E_FAIL;
	}
	if (FAILED(D3DXCreateSprite(pDevice, &pSprite)))
	{
		ERR_MSG(L"Sprite Createing Failed! - CGraphic_Device.cpp");
		return E_FAIL;
	}
	D3DXFONT_DESCW FontInfo;
	ZeroMemory(&FontInfo, sizeof(D3DXFONT_DESCW));

	FontInfo.Height = 20;
	FontInfo.Width = 10;
	FontInfo.Weight = FW_HEAVY;
	FontInfo.CharSet = HANGUL_CHARSET;
	lstrcpy(FontInfo.FaceName, L"메이플스토리");

	if (FAILED(D3DXCreateFontIndirect(pDevice, &FontInfo, &pFont)))
	{
		ERR_MSG(L"장치 초기화 실패");
		return E_FAIL;
	}
	if (FAILED(D3DXCreateLine(pDevice, &pLine)))
	{
		ERR_MSG(L"라인 객체 생성 실패");
		return E_FAIL;
	}

	return S_OK;
}

void GraphicDevice::Release()
{
	// 지우는 순서 유의할것 
	DXRelease(pLine);
	DXRelease(pFont);
	DXRelease(pSprite);
	DXRelease(pDevice);
	DXRelease(pSDK);
}

void GraphicDevice::RenderBegin()
{
	pDevice->Clear(0, nullptr, D3DCLEAR_TARGET |
		D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
		D3DCOLOR_ARGB(255, 0, 0, 0), 0.f, 0);

	pDevice->BeginScene();

	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void GraphicDevice::RenderEnd(HWND hWND /*= nullptr*/)
{
	pSprite->End();

	pDevice->EndScene();

	pDevice->Present(nullptr, nullptr, hWND, nullptr);
}

