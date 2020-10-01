#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	m_tInfo.vPos = {50.f, 50.f, 0.f}; 
	m_tInfo.vSize = D3DXVECTOR3(100.f, 100.f, 0.f); 
	m_tInfo.vDir = D3DXVECTOR3(0.f, -1.f, 0.f); 
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);

	// 좌상 
	m_vP[0] = _vec3(-m_tInfo.vSize.x * 0.5f, -m_tInfo.vSize.y * 0.5f, 0.f);
	// 우상y
	m_vP[1] = _vec3(m_tInfo.vSize.x * 0.5f, -m_tInfo.vSize.y * 0.5f, 0.f);
	//우하
	m_vP[2] = _vec3(m_tInfo.vSize.x * 0.5f, m_tInfo.vSize.y * 0.5f, 0.f);
	//좌하 
	m_vP[3] = _vec3(-m_tInfo.vSize.x * 0.5f, m_tInfo.vSize.y * 0.5f, 0.f);

	// 자 이건 위치값을 먹인 상태에서 회전을 하게 되면 일어나는 일을 보여주기 위해 요롷게 남겨둠. 
// 	for (int i = 0 ; i < 4;++i)
// 		m_vP[i] += m_tInfo.vPos; 

	return S_OK;
}

void CPlayer::Update_GameObject()
{

	//m_f두근두근 += 0.01f; 
	m_fAngle += 2.f; 
	D3DXMATRIX matScale, matRotZ, matTrans, matRevRotZ, matParent, matWorld;
	D3DXMATRIX matRotX, matRotY; 
	D3DXMatrixScaling(&matScale, m_f두근두근, m_f두근두근, 0.f);

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_fAngle)); 
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(m_fAngle));
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fAngle));
	
	D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f); 
	D3DXMatrixRotationZ(&matRevRotZ, D3DXToRadian(m_fAngle)); 
	D3DXMatrixTranslation(&matParent, 400.f, 300.f, 0.f); 
	matWorld = matScale /**matRotX * matRotY * matRotZ*/ * matTrans * matRevRotZ * matParent;

	for (int i = 0; i < 4; ++i)
		D3DXVec3TransformCoord(&m_vQ[i], &m_vP[i], &matWorld); 
// 	D3DXVec3TransformCoord();
// 	5	5	0	1		1	0	0	0
// 						0	1	0	0
// 						0	0	1	0
// 						2	2	0	1
// 	for (int i = 0 ; i < 4; ++i)
// 	{
// 		
// 		//점 Q의 X좌표는 x * cos@ - y * sin@
// 		//점 Q의 Y좌표는 x * sin@ + y * cos@
// 		m_vQ[i].x = m_vP[i].x * cosf(D3DXToRadian(m_fAngle)) - m_vP[i].y * sinf(D3DXToRadian(m_fAngle));
// 		m_vQ[i].y = m_vP[i].x * sinf(D3DXToRadian(m_fAngle)) + m_vP[i].y * cosf(D3DXToRadian(m_fAngle)); 
// 
// 		// 자전을 하고싶다면 회전이 진행된 이후에 위치값을 넣어주자. 
// 		m_vQ[i] += m_tInfo.vPos;
// 	}

}

void CPlayer::LateUpdate_GameObject()
{
	if (GetAsyncKeyState(VK_LEFT))
		m_tInfo.vPos.x -= 5.f;
	if (GetAsyncKeyState(VK_RIGHT))
		m_tInfo.vPos.x += 5.f;

	if (GetAsyncKeyState(VK_UP))
		m_tInfo.vPos.y -= 5.f; 
	if (GetAsyncKeyState(VK_DOWN))
		m_tInfo.vPos.y += 5.f;
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
D3DXMatrixIdentity(D3DXMATRIX *pOut); 항등행렬로 만들어주는 함수. 
항등행렬 - 행렬의 초기값이라 생각하면 편하다. 예를 들어 포인터는 nullptr로 초기화 하듯 행렬도 항등행렬로 초기화를 한다. 
항등행렬의 경우 행렬의 곱셈에서 교환법칙 성립된다. 
1	0	0	0
0	1	0	0
0	0	1	0
0	0	0	1
// 크기 관련 함수. 
D3DXMatrixScaling(D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz);
11, 22, 33번째 원소에 영향을 끼친다. 중요한건 크기 넣어주는것은 비율을 넣어줘야 한다. 
예를들어 2배 혹은 3배 요론식으로. 
*행렬 관련 함수들은 아웃풋으로 받은 행렬을 항등행렬로 만들고 연산을 수행한다. 
X	0	0	0
0	X	0	0
0	0	X	0
0	0	0	1

// 이동관련 행렬함수. 
D3DXMatrixTranslation( D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z )
*행렬 관련 함수들은 아웃풋으로 받은 행렬을 항등행렬로 만들고 연산을 수행한다.
1	0	0	0
0	1	0	0
0	0	1	0
X	X	X	1

//회전관련 행렬 함수 X,Y, Z
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

// 벡터와 행렬간의 곱셈에 사용되는 함수. 
D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
D3DXVec3TransformNormal(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);
*/

