
// MFCToolView.cpp : CMFCToolView Ŭ������ ����
//

#include "stdafx.h"
#include <queue>

// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "MFCTool.h"
#endif

#include "MFCToolDoc.h"
#include "MFCToolView.h"
#include "Single_Texture.h"
#include "Texture_Manager.h"
#include "Terrain.h"
#include "ObjectEdit.h"
#include "MainFrm.h"
#include "MiniView.h"
#include "MyForm.h"
#include <mutex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMFCToolView

IMPLEMENT_DYNCREATE(CMFCToolView, CScrollView)
BEGIN_MESSAGE_MAP(CMFCToolView, CScrollView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CMFCToolView ����/�Ҹ�

CMFCToolView::CMFCToolView()
{
	
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
}

CMFCToolView::~CMFCToolView()
{
}

BOOL CMFCToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CScrollView::PreCreateWindow(cs);
}

// CMFCToolView �׸���

void CMFCToolView::TileEditPushEvent(const CPoint point)
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	// ���콺 ��ũ�� ��ǥ�� ���� ��ǥ��
	const vec3 WorldPoint = ClientPosToJoomApplyWorldPosition(point);


	if (bRenderTileMode)
		MousePickPushTile(WorldPoint);
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
			_CollisionLineManager.Push({ std::move(LineFirst),std::move(LineSecond) },bWallRide);
		}
	}
	if (bObjectMode)
	{
		int32_t DrawID = pMyForm->_ObjectTool._DrawID;
		_ObjectEdit.PushObjectInStage(WorldPoint, DrawID);
	}
	
	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);
}

void CMFCToolView::TileEditEraseEvent(const CPoint point)
{
	const vec3 WorldPoint = ClientPosToJoomApplyWorldPosition(point);

	if (bRenderTileMode)
		MousePickDeleteTile(WorldPoint);
	if (bCollisionTileMode)
		_CollisionTileManager.Erase(WorldPoint);
	if (bLineMode)
		_CollisionLineManager.Erase(WorldPoint, bWallRide);
	if (bObjectMode)
		_ObjectEdit.DeleteObjAtPointLocation(WorldPoint);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm*	pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView*	 pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);
}

vec3 CMFCToolView::ClientPosToJoomApplyWorldPosition(const CPoint point)
{
	vec3 WorldPoint =
		vec3{static_cast<float>(point.x),static_cast<float>(point.y),0.f };

	vec3 ScreenCenterPoint = { global::ClientSize.first / 2.f,global::ClientSize.second / 2.f,0.f };
	vec3 ScrollPoint = { (float)GetScrollPos(0),(float)GetScrollPos(1),0.f };

	WorldPoint -= ScreenCenterPoint;
	WorldPoint /= JoomScale;
	WorldPoint += ScrollPoint;
	WorldPoint += ScreenCenterPoint;

	return WorldPoint;
}

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
	_ObjectEdit.Render();

	GraphicDevice::instance().RenderEnd();
}


// CMFCToolView �μ�

BOOL CMFCToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMFCToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMFCToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CMFCToolView ����

#ifdef _DEBUG
void CMFCToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMFCToolView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMFCToolDoc* CMFCToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCToolDoc)));
	return (CMFCToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCToolView �޽��� ó����

void CMFCToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	global::hWND = m_hWnd;

	GraphicDevice::instance().Ready();
	
	SetScrollSizes(MM_TEXT,

		CSize{  int32_t(global::TileSize.first * global::TileNums.first*2.0 + global::ClientViewMinLeftTop) ,
				int32_t(global::TileSize.second* global::TileNums.second*2.0 + global::ClientViewMinLeftTop), }

	);

	// //1.���� ������ ȹ��
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

	// ���⼭ �ؽ��ĸ� �̸� �ε��մϴ�.
#pragma region TEXTURE_LOAD
	{

		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Prison/%d.png", L"Map", L"Prison", 27)))
			return;

		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Multiplayer/%d.png", L"Map", L"Multiplayer", 376)))
			return;
		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Bunkermansion/%d.png", L"Map", L"Bunkermansion", 438)))
			return;

		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Stage01/%d.png", L"Map", L"Stage1", 2)))
			return;
		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/Stage02/%d.png", L"Map", L"Stage2", 2)))
			return;
																		
		if (FAILED(Texture_Manager::instance().InsertTexture(Texture_Manager::MULTI_TEX,
			L"../Resource/Texture/EditObject/Object%d.png", L"ObjectPicture", L"ObjectPicture1", 9)))
			return;
	}
#pragma endregion _TEXTURE_LOAD_END
	
	
	if (nullptr == up_Terrain)
	{
		up_Terrain = std::make_unique<Terrain>();
		up_Terrain->Set_View(this);
	}

	_ObjectEdit.Set_View(this);
};

void CMFCToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	TileEditPushEvent(point);

	CScrollView::OnLButtonDown(nFlags, point);
};

void CMFCToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (!up_Terrain)return;

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{

		// ���콺 ��ũ�� ��ǥ�� ���� ��ǥ��
		const vec3 WorldPoint = ClientPosToJoomApplyWorldPosition(point);


		if (bRenderTileMode)
			MousePickPushTile(WorldPoint);
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
				_CollisionLineManager.Push({ std::move(LineFirst),std::move(LineSecond) },bWallRide);
			}
		}
	}
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		const vec3 WorldPoint = ClientPosToJoomApplyWorldPosition(point);

		if (bRenderTileMode)
			MousePickDeleteTile(WorldPoint);
		if (bCollisionTileMode)
			_CollisionTileManager.Erase(WorldPoint);
		if (bLineMode)
			_CollisionLineManager.Erase(WorldPoint, bWallRide);
	}

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);

	CScrollView::OnMouseMove(nFlags, point);
}

void CMFCToolView::MousePickPushTile(const vec3& WorldPos)
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	int32_t DrawID = pMyForm->_MapTool._DrawID;
	ELayer_Map CurrentSelectLayerMap = pMyForm->_MapTool._CurrentSelectLayerMap;

	up_Terrain->PickingPushMapObj(WorldPos, DrawID, CurrentSelectLayerMap);

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);
}
void CMFCToolView::MousePickDeleteTile(const vec3& WorldPos)
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMain->_SecondSplitter.GetPane(0, 0));

	ELayer_Map CurrentSelectLayerMap = pMyForm->_MapTool._CurrentSelectLayerMap;

	up_Terrain->DeleteMapObjAtPointLocation(WorldPos, CurrentSelectLayerMap);

	InvalidateRect(nullptr, FALSE);
	pMiniView->InvalidateRect(nullptr, FALSE);
}
void CMFCToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	TileEditEraseEvent(point);
	CScrollView::OnRButtonDown(nFlags, point);
}
void CMFCToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if (nChar == 'P')
		global::bDebug = !global::bDebug;

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
BOOL CMFCToolView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	constexpr float MouseWheelResponsiVeness = 1.f;

	//pt.x += GetScrollPos(0);
	//pt.y += GetScrollPos(1);

	//const float CurrentMousePosScrollX = max(0.f, pt.x - global::ClientSize.first / 2.f);
	//const float CurrentMousePosScrollY = max(0.f, pt.y - global::ClientSize.second / 2.f);

	//SetScrollPos(0, CurrentMousePosScrollX, TRUE);
	//SetScrollPos(1, CurrentMousePosScrollY, TRUE);

	JoomScale = max(1.f, ( (zDelta / 1200.f)*MouseWheelResponsiVeness) + JoomScale);

	SetScrollSizes(MM_TEXT,

		CSize{ int32_t(global::TileSize.first * global::TileNums.first*max(2.0,JoomScale )  +global::ClientViewMinLeftTop),
				int32_t(global::TileSize.second* global::TileNums.second*max(2.0,JoomScale ) + global::ClientViewMinLeftTop), }

	);
	InvalidateRect(nullptr, TRUE);

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}
