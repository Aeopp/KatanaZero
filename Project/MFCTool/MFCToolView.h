
// MFCToolView.h : CMFCToolView Ŭ������ �������̽�
//

#pragma once



#include "CollisionTileManager.h"
#include "CollisionLineManager.h"

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

// �۾��Դϴ�.
public:
	bool bRenderTileMode = false;
	bool bCollisionTileMode = false;
	bool bLineMode = false;

	CollisionLineManager _CollisionLineManager; 
	CollisionTileManager _CollisionTileManager;
	std::unique_ptr<class Terrain> up_Terrain{ nullptr };
	float Angle = 0.f; 
// �������Դϴ�.
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
	void MousePickPushTile(const CPoint& point);
	void MousePickDeleteTile(const CPoint& point);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // MFCToolView.cpp�� ����� ����
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

