#include "stdafx.h"
#include "Graphic_Device.h"


GraphicDevice::~GraphicDevice()
{
	Release(); 
}

HRESULT GraphicDevice::Initialize()
{
	// ��ġ�� �����ϴ� ����. 
	// 1.��ġ�� ������ �İ�ü(m_pSDK) ����. 
	// 2. ��ġ�� ���� ����, 
	// 3. pSDK�� �̿��Ͽ� ��ġ�� �����ϴ� �İ�ü(m_pDevice)�� ����.

	D3DCAPS9 DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9)); 

	pSDK = Direct3DCreate9(D3D_SDK_VERSION); 
	// ���� ���̷�Ʈ ������ �´� �İ�ü�� ����

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
	d3dpp.BackBufferHeight = global::ClientSize.second; // ����� ���μ��� ũ�� ����. 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = global::hWND;
	d3dpp.Windowed = TRUE;// â����� ��� true, ��üȭ���� ��� false, 
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
	lstrcpy(FontInfo.FaceName,L"�����ý��丮");

	if (FAILED(D3DXCreateFontIndirect(pDevice, &FontInfo, &pFont)))
	{
		ERR_MSG(L"��ġ �ʱ�ȭ ����"); 
		return E_FAIL; 
	}
	if (FAILED(D3DXCreateLine(pDevice, &pLine)))
	{
		ERR_MSG(L"���� ��ü ���� ����");
		return E_FAIL;
	}

	return S_OK;
}

void GraphicDevice::Release()
{
	// ����� ���� �����Ұ� 
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
