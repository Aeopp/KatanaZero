#include "stdafx.h"
#include "MainApp.h"
#include "Player.h"

CMainApp::CMainApp()
{
}



HRESULT CMainApp::Ready_MainApp()
{
	m_hDC = GetDC(g_hWND);
	m_pPlayer = CPlayer::Create();
	if (m_pPlayer == nullptr)
		return E_FAIL; 

	return S_OK; 

}

void CMainApp::Update_MainApp()
{
	m_pPlayer->Update_GameObject(); 
}

void CMainApp::LateUpdate_MainApp()
{
	m_pPlayer->LateUpdate_GameObject();
}

void CMainApp::Render_MainApp()
{
	Rectangle(m_hDC, 0, 0, g_iWinCX, g_iWinCY);
	m_pPlayer->Render_GameObject(m_hDC);
}

void CMainApp::Release_MainApp()
{

}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp; 
	if (FAILED(pInstance->Ready_MainApp()))
	{
		delete pInstance; 
		return nullptr;
	}
	return pInstance;
}

void CMainApp::Free()
{
	ReleaseDC(g_hWND, m_hDC); 
	SAFE_DELETE(m_pPlayer);
}
