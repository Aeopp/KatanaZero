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
	float m_f두근두근 = 1.f; 
	RECT m_tRect; 
	vec3 m_tInfo;

	vec3 m_vP[4]; // 원점 기준. 
	vec3 m_vQ[4]; // 돌아간놈. 
public :
	static CPlayer* Create(); 
	void Free(); 
};

