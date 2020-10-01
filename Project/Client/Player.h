#pragma once
class CPlayer final
{
private:
	explicit CPlayer();
	virtual ~CPlayer();
public :
	HRESULT Ready_GameObject(); 
	void Update_GameObject();
	void LateUpdate_GameObject();
	void Render_GameObject(HDC hDC);
	void Release_GameObject();

private:
	float m_fAngle = 0;
	float m_f�αٵα� = 1.f; 
	INFO m_tInfo; 
	RECT m_tRect; 
	_vec3 m_vP[4]; // ���� ����. 
	_vec3 m_vQ[4]; // ���ư���. 
public :
	static CPlayer* Create(); 
	void Free(); 



};

