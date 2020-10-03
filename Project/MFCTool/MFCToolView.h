
// MFCToolView.h : CMFCToolView 클래스의 인터페이스
//

#pragma once



#include "CollisionTileManager.h"
#include "CollisionLineManager.h"

// 이제 스크롤을 사용하기 위해서 원래 상속받고 있던 cView가 아니라 CScrollView로 바꿔줌,. 
// 바꿔주면서 원래 쓰던 CView를 전부 CScrollView로 바꿔주자!!! 
class CMFCToolView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CMFCToolView();
	DECLARE_DYNCREATE(CMFCToolView)

// 특성입니다.
public:
	class CMFCToolDoc* GetDocument() const;

// 작업입니다.
public:
	bool bRenderTileMode = false;
	bool bCollisionTileMode = false;
	bool bLineMode = false;

	CollisionLineManager _CollisionLineManager; 
	CollisionTileManager _CollisionTileManager;
	std::unique_ptr<class Terrain> up_Terrain{ nullptr };
	float Angle = 0.f; 
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMFCToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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

#ifndef _DEBUG  // MFCToolView.cpp의 디버그 버전
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

