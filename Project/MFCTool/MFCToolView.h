
// MFCToolView.h : CMFCToolView Ŭ������ �������̽�
//

#pragma once

class CGraphic_Device;

#include "CollisionTileManager.h"
#include "CollisionLineManager.h"
#include <queue>
#include "ObjectEdit.h"

// ���� ��ũ���� ����ϱ� ���ؼ� ���� ��ӹް� �ִ� cView�� �ƴ϶� CScrollView�� �ٲ���,. 
// �ٲ��ָ鼭 ���� ���� CView�� ���� CScrollView�� �ٲ�����!!! 
class CMFCToolView : public CScrollView
{
protected: // serialization������ ��������ϴ�.
	CMFCToolView();
	DECLARE_DYNCREATE(CMFCToolView)

// Ư���Դϴ�.
public:
	class CMFCToolDoc* GetDocument() const;

	CGraphic_Device* m_pGraphic_Device;

// �۾��Դϴ�.
public:
	ObjectEdit _ObjectEdit;
	bool bRenderTileMode = false;
	bool bCollisionTileMode = false;
	bool bLineMode = false;
	bool bWallRide= false;
	bool bObjectMode = false;
	bool bTileCanGoDown = false;

	CollisionLineManager _CollisionLineManager; 
	CollisionTileManager _CollisionTileManager;
	std::unique_ptr<class Terrain> up_Terrain{ nullptr };
	float Angle = 0.f; 
	float JoomScale = 1.f;
// �������Դϴ�.
private:
	std::queue<vec3> _LinePointQ{};
private:
	void TileEditPushEvent(const CPoint point);
	void TileEditEraseEvent(const CPoint point);
	vec3 ClientPosToJoomApplyWorldPosition(const CPoint point);
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CMFCToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
	void MousePickPushTile(const vec3& WorldPos);
	void MousePickDeleteTile(const vec3& WorldPos);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // MFCToolView.cpp�� ����� ����
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

