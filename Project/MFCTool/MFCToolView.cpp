
// MFCToolView.cpp : CMFCToolView 클래스의 구현
//

#include "stdafx.h"
#include <queue>

// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCTool.h"
#endif

#include "MFCToolDoc.h"
#include "MFCToolView.h"
#include "Single_Texture.h"
#include "Texture_Manager.h"
#include "Terrain.h"
#include "MainFrm.h"
#include "MiniView.h"
#include "MyForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMFCToolView

IMPLEMENT_DYNCREATE(CMFCToolView, CScrollView)
BEGIN_MESSAGE_MAP(CMFCToolView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CMFCToolView 생성/소멸

CMFCToolView::CMFCToolView()
{
	
	// TODO: 여기에 생성 코드를 추가합니다.
}

CMFCToolView::~CMFCToolView()
{
}

BOOL CMFCToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

// CMFCToolView 그리기

void CMFCToolView::OnDraw(CDC* /*pDC*/)
{
	CMFCToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (!up_Terrain)return;

	GraphicDevice::instance().RenderBegin();

	
	up_Terrain->Render();
	_CollisionTileManager.DebugRender();
	_CollisionLineManager.DebugRender();

	GraphicDevice::instance().RenderEnd();
}


// CMFCToolView 인쇄

BOOL CMFCToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMFCToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMFCToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CMFCToolView 진단

#ifdef _DEBUG
void CMFCToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMFCToolView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMFCToolDoc* CMFCToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCToolDoc)));
	return (CMFCToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCToolView 메시지 처리기

void CMFCToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	global::hWND = m_hWnd;

	GraphicDevice::instance().Ready();
	
	SetScrollSizes(MM_TEXT,

		CSize{  int32_t(global::TileSize.first * global::TileNums.first),
				int32_t(global::TileSize.second* global::TileNums.second), }

	);


	// //1.메인 프레임 획득
	CMainFrame* pMain = (CMainFrame*)::AfxGetApp()->GetMainWnd();

	RECT rcMain = {};
	pMain->GetWindowRect(&rcMain); 
	::SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	RECT rcView = {}; 
	GetClientRect(&rcView); 
	
	uint32_t Width = rcMain.right - rcView.right;
	uint32_t Height = rcMain.bottom - rcView.bottom;

	pMain->SetWindowPos(nullptr, 0, 0, global::ClientSize.first + Width,
										global::ClientSize.second + Height, 
										SWP_NOZORDER);

	// 여기서 텍스쳐를 미리 로딩합니다.
#pragma region TEXTURE_LOAD
	{

		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Prison/%d.png", L"Map", L"Prison", 27)))
			return;

		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Factory/%d.png", L"Map", L"Factory", 21)))
			return;
		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Mansion/%d.png", L"Map", L"Mansion", 38)))
			return;

	}
#pragma endregion _TEXTURE_LOAD_END
	
	
	if (nullptr == up_Terrain)
	{
		up_Terrain = std::make_unique<Terrain>();
		up_Terrain->Set_View(this);
	}
};

void CMFCToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	static std::queue<vec3> _LinePointQ{};
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	// 마우스 스크린 좌표를 월드 좌표로
	const vec3 WorldPoint = 
		vec3{ 
		(float)point.x + GetScrollPos(0),
		(float)point.y + GetScrollPos(1),
		0.f  };

	if (bRenderTileMode)
		MousePickPushTile(point);
	if (bCollisionTileMode)
	{
		_CollisionTileManager.Push(WorldPoint);
	}

	if (bLineMode)
	{
		_LinePointQ.push(WorldPoint);

		if (_LinePointQ.size() >= 2)
		{
			vec3 LineFirst = _LinePointQ.front();
			_LinePointQ.pop();
			vec3 LineSecond = _LinePointQ.front();
			_LinePointQ.pop();
			_CollisionLineManager.Push({std::move(LineFirst),std::move(LineSecond)});
		}
	}
		
	
	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);

	CScrollView::OnLButtonDown(nFlags, point);
};


void CMFCToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (!up_Terrain)return;

	static std::queue<vec3> _LinePointQ{};

	const vec3 WorldPoint =
		vec3{
		(float)point.x + GetScrollPos(0),
		(float)point.y + GetScrollPos(1),
		0.f };

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (bRenderTileMode)
			MousePickPushTile(point);
		if (bCollisionTileMode)
		{
			_CollisionTileManager.Push(WorldPoint);
		}

		if (bLineMode)
		{
			_LinePointQ.push(WorldPoint);

			if (_LinePointQ.size() >= 2)
			{
				vec3 LineFirst = _LinePointQ.front();
				_LinePointQ.pop();
				vec3 LineSecond = _LinePointQ.front();
				_LinePointQ.pop();
				_CollisionLineManager.Push({ std::move(LineFirst),std::move(LineSecond) });
			}
		}
	}
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		if (bRenderTileMode)
			MousePickDeleteTile(point);
		if (bCollisionTileMode)
			_CollisionTileManager.Erase(WorldPoint);
		if (bLineMode)
			_CollisionLineManager.Erase(WorldPoint);
	}
		
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);

	CScrollView::OnMouseMove(nFlags, point);
}


void CMFCToolView::MousePickPushTile(const CPoint & point)
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	int32_t DrawID = pMyForm->_MapTool._DrawID;
	ELayer_Map CurrentSelectLayerMap = pMyForm->_MapTool._CurrentSelectLayerMap;

	vec3 vMouse = { (float)point.x + GetScrollPos(0), (float)point.y + GetScrollPos(1), 0.f };

	up_Terrain->PickingPushMapObj(vMouse, DrawID, CurrentSelectLayerMap);

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);
}

void CMFCToolView::MousePickDeleteTile(const CPoint & point)
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	vec3 vMouse = { (float)point.x + GetScrollPos(0), (float)point.y + GetScrollPos(1), 0.f };

	up_Terrain->DeleteMapObjAtPointLocation(vMouse);

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);
}


void CMFCToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	const vec3 WorldPoint =
		vec3{
		(float)point.x + GetScrollPos(0),
		(float)point.y + GetScrollPos(1),
		0.f };

	if(bRenderTileMode)
		MousePickDeleteTile(point);
	if(bCollisionTileMode)
		_CollisionTileManager.Erase(WorldPoint);
	if (bLineMode)
		_CollisionLineManager.Erase(WorldPoint);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm*	pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView*	 pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);

	CScrollView::OnRButtonDown(nFlags, point);
}


void CMFCToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nChar == 'P')
		global::bDebug = !global::bDebug;

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


