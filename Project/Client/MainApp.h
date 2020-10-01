#pragma once
class CPlayer; 
class CMainApp
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default; //== virtual ~CMainApp() = default; 

public :
	HRESULT Ready_MainApp();
	void Update_MainApp(); 
	void LateUpdate_MainApp(); 
	void Render_MainApp(); 
	void Release_MainApp(); 

public :
	static CMainApp* Create(); 
	void Free(); 
private:
	HDC m_hDC; 
	CPlayer* m_pPlayer; 
};

