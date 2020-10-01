#pragma once

extern HWND g_hWND; 
const int g_iWinCX = 800; 
const int g_iWinCY = 600; 
typedef D3DXVECTOR3 _vec3;
typedef struct tagInfo
{
	D3DXVECTOR3 vPos; // 위치를 표현하기 위한 벡터. 
	D3DXVECTOR3 vDir; // 방향을 표현하기 위한 방향벡터.
	D3DXVECTOR3 vSize; // 
	D3DXVECTOR3 vLook; // 그냥 기준이 되는 축. 
	
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