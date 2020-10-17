#include "stdafx.h"
#include "Player.h"


HRESULT CPlayer::Ready_GameObject()
{
	//m_tInfo.vPos = {50.f, 50.f, 0.f}; 
	//m_tInfo.vSize = D3DXVECTOR3(100.f, 100.f, 0.f); 
	//m_tInfo.vDir = D3DXVECTOR3(0.f, -1.f, 0.f); 
	//m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);

	//// �»� 
	//m_vP[0] = vec3(-m_tInfo.vSize.x * 0.5f, -m_tInfo.vSize.y * 0.5f, 0.f);
	//// ���y
	//m_vP[1] = vec3(m_tInfo.vSize.x * 0.5f, -m_tInfo.vSize.y * 0.5f, 0.f);
	////����
	//m_vP[2] = vec3(m_tInfo.vSize.x * 0.5f, m_tInfo.vSize.y * 0.5f, 0.f);
	////���� 
	//m_vP[3] = vec3(-m_tInfo.vSize.x * 0.5f, m_tInfo.vSize.y * 0.5f, 0.f);

	// �� �̰� ��ġ���� ���� ���¿��� ȸ���� �ϰ� �Ǹ� �Ͼ�� ���� �����ֱ� ���� ���� ���ܵ�. 
// 	for (int i = 0 ; i < 4;++i)
// 		m_vP[i] += m_tInfo.vPos; 

	return S_OK;
}

void CPlayer::Update_GameObject()
{

	////m_f�αٵα� += 0.01f; 
	//m_fAngle += 2.f; 
	//D3DXMATRIX matScale, matRotZ, matTrans, matRevRotZ, matParent, matWorld;
	//D3DXMATRIX matRotX, matRotY; 
	//D3DXMatrixScaling(&matScale, m_f�αٵα�, m_f�αٵα�, 0.f);

	//D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_fAngle)); 
	//D3DXMatrixRotationY(&matRotY, D3DXToRadian(m_fAngle));
	//D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fAngle));
	//
	//D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f); 
	//D3DXMatrixRotationZ(&matRevRotZ, D3DXToRadian(m_fAngle)); 
	//D3DXMatrixTranslation(&matParent, 400.f, 300.f, 0.f); 
	//matWorld = matScale /**matRotX * matRotY * matRotZ*/ * matTrans * matRevRotZ * matParent;

	//for (int i = 0; i < 4; ++i)
	//	D3DXVec3TransformCoord(&m_vQ[i], &m_vP[i], &matWorld); 
// 	D3DXVec3TransformCoord();
// 	5	5	0	1		1	0	0	0
// 						0	1	0	0
// 						0	0	1	0
// 						2	2	0	1
// 	for (int i = 0 ; i < 4; ++i)
// 	{
// 		
// 		//�� Q�� X��ǥ�� x * cos@ - y * sin@
// 		//�� Q�� Y��ǥ�� x * sin@ + y * cos@
// 		m_vQ[i].x = m_vP[i].x * cosf(D3DXToRadian(m_fAngle)) - m_vP[i].y * sinf(D3DXToRadian(m_fAngle));
// 		m_vQ[i].y = m_vP[i].x * sinf(D3DXToRadian(m_fAngle)) + m_vP[i].y * cosf(D3DXToRadian(m_fAngle)); 
// 
// 		// ������ �ϰ�ʹٸ� ȸ���� ����� ���Ŀ� ��ġ���� �־�����. 
// 		m_vQ[i] += m_tInfo.vPos;
// 	}

}

void CPlayer::LateUpdate_GameObject()
{
	/*if (GetAsyncKeyState(VK_LEFT))
		m_tInfo.vPos.x -= 5.f;
	if (GetAsyncKeyState(VK_RIGHT))
		m_tInfo.vPos.x += 5.f;

	if (GetAsyncKeyState(VK_UP))
		m_tInfo.vPos.y -= 5.f; 
	if (GetAsyncKeyState(VK_DOWN))
		m_tInfo.vPos.y += 5.f;*/
}

void CPlayer::Render_GameObject(HDC hDC)
{
	MoveToEx(hDC, m_vQ[0].x, m_vQ[0].y, nullptr); 
	for (int i = 1 ; i < 4 ; ++i)
	{
		LineTo(hDC, m_vQ[i].x, m_vQ[i].y);
	}
	LineTo(hDC, m_vQ[0].x, m_vQ[0].y); 
// 	m_tRect.left = m_tInfo.vPos.x - (static_cast<long>(m_tInfo.vSize.x) >> 1);
// 	m_tRect.right = m_tInfo.vPos.x + (m_tInfo.vSize.x * 0.5f);
// 	m_tRect.bottom = m_tInfo.vPos.y + (m_tInfo.vSize.y / 2.f);
// 	m_tRect.top = m_tInfo.vPos.y - (LONG(m_tInfo.vSize.y) >> 1);
// 
// 	Rectangle(hDC, m_tRect.left, m_tRect.top, m_tRect.right, m_tRect.bottom);
}

void CPlayer::Release_GameObject()
{
}

CPlayer * CPlayer::Create()
{
	CPlayer* pInstance = new CPlayer; 
	if (FAILED(pInstance->Ready_GameObject()))
	{
		delete pInstance; 
		pInstance = nullptr; 
	}
	return pInstance;
}

void CPlayer::Free()
{
	D3DXMATRIX mat; 
	D3DXMatrixScaling(&mat, 2.f, 2.f, 0.f); 

}

/*
D3DXMatrixIdentity(D3DXMATRIX *pOut); �׵���ķ� ������ִ� �Լ�. 
�׵���� - ����� �ʱⰪ�̶� �����ϸ� ���ϴ�. ���� ��� �����ʹ� nullptr�� �ʱ�ȭ �ϵ� ��ĵ� �׵���ķ� �ʱ�ȭ�� �Ѵ�. 
�׵������ ��� ����� �������� ��ȯ��Ģ �����ȴ�. 
1	0	0	0
0	1	0	0
0	0	1	0
0	0	0	1
// ũ�� ���� �Լ�. 
D3DXMatrixScaling(D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz);
11, 22, 33��° ���ҿ� ������ ��ģ��. �߿��Ѱ� ũ�� �־��ִ°��� ������ �־���� �Ѵ�. 
������� 2�� Ȥ�� 3�� ��н�����. 
*��� ���� �Լ����� �ƿ�ǲ���� ���� ����� �׵���ķ� ����� ������ �����Ѵ�. 
X	0	0	0
0	X	0	0
0	0	X	0
0	0	0	1

// �̵����� ����Լ�. 
D3DXMatrixTranslation( D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z )
*��� ���� �Լ����� �ƿ�ǲ���� ���� ����� �׵���ķ� ����� ������ �����Ѵ�.
1	0	0	0
0	1	0	0
0	0	1	0
X	X	X	1

//ȸ������ ��� �Լ� X,Y, Z
D3DXMatrixRotationX( D3DXMATRIX *pOut, FLOAT Angle )
1		0		0		0
0		cos		sin		0
0		-sin	cos		0
0		0		0		1	

D3DXMatrixRotationY(D3DXMATRIX *pOut, FLOAT Angle)
cos		0		-sin	0
0		1		0		0
sin		0		cos		0
0		0		0		1

D3DXMatrixRotationZ(D3DXMATRIX *pOut, FLOAT Angle)
cos		sin		0		0
-sin	cos		0		0
0		0		1		0
0		0		0		1

// ���Ϳ� ��İ��� ������ ���Ǵ� �Լ�. 
D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
D3DXVec3TransformNormal(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
*/

