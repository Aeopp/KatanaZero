
// MFCToolView.h : CMFCToolView 클래스의 인터페이스
//

#pragma once

class CGraphic_Device;

#include "CollisionTileManager.h"
#include "CollisionLineManager.h"
#include <queue>
#include "ObjectEdit.h"

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

	CGraphic_Device* m_pGraphic_Device;

// 작업입니다.
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
// 재정의입니다.
private:
	std::queue<vec3> _LinePointQ{};
private:
	void TileEditPushEvent(const CPoint point);
	void TileEditEraseEvent(const CPoint point);
	vec3 ClientPosToJoomApplyWorldPosition(const CPoint point);
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
	void MousePickPushTile(const vec3& WorldPos);
	void MousePickDeleteTile(const vec3& WorldPos);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // MFCToolView.cpp의 디버그 버전
inline CMFCToolDoc* CMFCToolView::GetDocument() const
   { return reinterpret_cast<CMFCToolDoc*>(m_pDocument); }
#endif

