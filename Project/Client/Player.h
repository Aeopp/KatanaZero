#pragma once
class CPlayer final
{
public :
	HRESULT Ready_GameObject(); 
	void Update_GameObject();
	void LateUpdate_GameObject();
	void Render_GameObject(HDC hDC);
	void Release_GameObject();

private:
	float m_fAngle = 0;
	float m_f�αٵα� = 1.f; 
	RECT m_tRect; 
	vec3 m_tInfo;

	vec3 m_vP[4]; // ���� ����. 
	vec3 m_vQ[4]; // ���ư���. 
public :
	static CPlayer* Create(); 
	void Free(); 
};

