#pragma once

extern HWND g_hWND; 
const int g_iWinCX = 800; 
const int g_iWinCY = 600; 
typedef D3DXVECTOR3 _vec3;
typedef struct tagInfo
{
	D3DXVECTOR3 vPos; // ��ġ�� ǥ���ϱ� ���� ����. 
	D3DXVECTOR3 vDir; // ������ ǥ���ϱ� ���� ���⺤��.
	D3DXVECTOR3 vSize; // 
	D3DXVECTOR3 vLook; // �׳� ������ �Ǵ� ��. 
	
}INFO;

template<typename T>
void SAFE_DELETE(T& Instance)
{
	if (Instance)
	{
		Instance->Free(); 
		Instance = nullptr; 
	}
}