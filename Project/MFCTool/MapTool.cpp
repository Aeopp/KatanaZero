// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "Terrain.h"

// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAPTOOL, pParent)
{

}

CMapTool::~CMapTool()
{
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, _ListBox);
	DDX_Control(pDX, IDC_PICTURE, _Picture);

	DDX_Control(pDX, IDC_RADIO2, MapTexStateKeyRadioBtnMap[L"Prison"s].first);
	DDX_Control(pDX, IDC_RADIO3, MapTexStateKeyRadioBtnMap[L"Multiplayer"s].first);
	DDX_Control(pDX, IDC_RADIO1, MapTexStateKeyRadioBtnMap[L"Bunkermansion"s].first);
	DDX_Control(pDX, IDC_RADIO4, MapTexStateKeyRadioBtnMap[L"Chinatown"s].first);
	DDX_Control(pDX, IDC_RADIO6, MapTexStateKeyRadioBtnMap[L"Boss"s].first);
	DDX_Control(pDX, IDC_RADIO5, MapTexStateKeyRadioBtnMap[L"Stage1"s].first);
	DDX_Control(pDX, IDC_RADIO7, MapTexStateKeyRadioBtnMap[L"Stage2"s].first);

	// 레이어 버튼 테이블 세팅
	DDX_Control(pDX, IDC_RADIO8, MapLayerSelectRadioBtnTable[ELayer_Map::BACK_GROUND]);
	DDX_Control(pDX, IDC_RADIO10, MapLayerSelectRadioBtnTable[ELayer_Map::FORE_GROUND]);
	DDX_Control(pDX, IDC_RADIO9, MapLayerSelectRadioBtnTable[ELayer_Map::DECORATIONS]);

	DDX_Control(pDX, IDC_CHECK3, CheckBoxRenderTile);
	DDX_Control(pDX, IDC_CHECK1, CheckBoxCollisionTile);
	DDX_Control(pDX, IDC_CHECK2, CheckBoxLine);
	DDX_Control(pDX, IDC_CHECK4, CheckBoxObjectMode);
	DDX_Control(pDX, IDC_CHECK5, CheckBoxWallRide);
	DDX_Control(pDX, IDC_CHECK6, CheckBoxTileCanGoDown);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnLbnSelchangePicture)
	ON_BN_CLICKED(IDC_BUTTON1, &CMapTool::OnBnClickedMapSaveButton)
	ON_BN_CLICKED(IDC_BUTTON2, &CMapTool::OnBnClickedMapLoadButton)

	ON_BN_CLICKED(IDC_RADIO2, &CMapTool::OnMapStateRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO3, &CMapTool::OnMapStateRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO1, &CMapTool::OnMapStateRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO4, &CMapTool::OnMapStateRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO6, &CMapTool::OnMapStateRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO5, &CMapTool::OnMapStateRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO7, &CMapTool::OnMapStateRadioBtnClickEvent)

	ON_BN_CLICKED(IDC_CHECK3, &CMapTool::OnBnClickedCheckRenderTile)
	ON_BN_CLICKED(IDC_CHECK1, &CMapTool::OnBnClickedCheckCollisionTile)
	ON_BN_CLICKED(IDC_CHECK2, &CMapTool::OnBnClickedCheckLine)
	ON_BN_CLICKED(IDC_CHECK4, &CMapTool::OnBnClickedCheckObjectToggle)
	// 여기에 함수를 바인딩 합니다.
	ON_BN_CLICKED(IDC_RADIO8, &CMapTool::OnMapLayerRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO10, &CMapTool::OnMapLayerRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_RADIO9, &CMapTool::OnMapLayerRadioBtnClickEvent)
	ON_BN_CLICKED(IDC_CHECK5, &CMapTool::OnBnClickedCheckWallRide)
	ON_BN_CLICKED(IDC_CHECK6, &CMapTool::OnBnClickedCheckTileCanGoDown)
END_MESSAGE_MAP()

// CMapTool 메시지 처리기입니다.
// 해당 함수 호출 전 파일은 로딩이 끝난 상태여야 합니다.
void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	// 리스트 박스를 초기화 합니다. 
	_ListBox.ResetContent();
	//////////
	
	// 현재 선택된 라디오 버튼의 문자열을 찾습니다.
	std::wstring CurrentSelectRadioBtnStr; 
	for (const auto& StateKey_RadioBtnFileNameSet : MapTexStateKeyRadioBtnMap)
	{
		const auto& RadioBtn = StateKey_RadioBtnFileNameSet.second.first;
		if (RadioBtn.GetCheck())
		{
			CString CurrentRadioBtnText{};
			RadioBtn.GetWindowText(CurrentRadioBtnText);
			CurrentSelectRadioBtnStr = CurrentRadioBtnText.GetString();
		}
	}

	TCHAR FilePath[MAX_PATH] = L"";
	TCHAR FileName[MAX_PATH] = L"";
	CString ReletivePath = L""; 
	int iSize = DragQueryFile(hDropInfo, -1, nullptr, 0);
	CString FileNameAndExtant = L""; 
	for (int i = 0; i < iSize; ++i)
	{
		DragQueryFile(hDropInfo, i, FilePath, MAX_PATH);
		ReletivePath = FileHelper::ConvertReletivePath(FilePath);
		FileNameAndExtant = PathFindFileName(ReletivePath);
		lstrcpy(FileName, FileNameAndExtant.GetString());
		PathRemoveExtension(FileName);
		_ListBox.AddString(FileName);
		MapTexStateKeyRadioBtnMap[CurrentSelectRadioBtnStr].second.emplace(FileName);
	}

	HorizontalScroll();

	UpdateData(FALSE); 
	CDialog::OnDropFiles(hDropInfo);
}

void CMapTool::HorizontalScroll()
{
	CString Name; 
	CSize Size; 
	int CX = 0; 
	CDC* pDC = _ListBox.GetDC();

	for (int i = 0 ; i < _ListBox.GetCount(); ++i)
	{
		_ListBox.GetText(i, Name);
		Size = pDC->GetTextExtent(Name);

		if (Size.cx > CX)
			CX = Size.cx;
	}

	_ListBox.ReleaseDC(pDC); 

	if (CX > _ListBox.GetHorizontalExtent())
	{
		_ListBox.SetHorizontalExtent(CX);
	}
}

// TODO :: 여기에 타일 파일 선택후 우측 미니뷰의 그리기 코드를 작성해주세요.
// 해당 함수의 호출 타이밍은 리스트 박스의 타일을 선택하는 순간입니다.
void CMapTool::OnLbnSelchangePicture()
{
	constexpr float SelectPictureViewScale = 0.9f;

	auto* pView = GetView();
	if (!pView)return;
	if (!pView->up_Terrain)return;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CString CurrentSelectName = L"";
	// 제로인덱스부터 시작하는 리스트 박스의 현재 선택한 인덱스입니다 (파일명과 혼동하지 마세요..)
	int32_t SelectFileIndex = _ListBox.GetCurSel();

	if (LB_ERR == SelectFileIndex)
		return;

	// 리스트박스의 현재선택 인덱스로부터 문자열을 얻어옵니다.
	_ListBox.GetText(SelectFileIndex, CurrentSelectName);

	// 파일명중 숫자만 남기기 위해 추적합니다.
	int32_t i = CurrentSelectName.GetLength() - 1;
	for (; i >= 0; --i)
	{
		if (false == isdigit(CurrentSelectName[i]))
			break;
	}
	// 해당 작업 이후 파일명에서 숫자만 남습니다.
	CurrentSelectName.Delete(0, i+1);

	// 숫자 문자열을 실제 정수로 변환합니다.
	_DrawID = _wtoi(CurrentSelectName.GetString());

	GraphicDevice::instance().RenderBegin();

	matrix MScale, MTranslation, MWorld;
	auto sp_TexInfo = Texture_Manager::instance().
		Get_TexInfo(L"Map", pView->up_Terrain->CurrentTileTextureStateKey, _DrawID);

	if (nullptr == sp_TexInfo)
		return;

	float CenterX = sp_TexInfo->ImageInfo.Width >> 1;
	float CenterY = sp_TexInfo->ImageInfo.Height >> 1;

	const float ClientSizeX = global::ClientSize.first;
	const float ClientSizeY = global::ClientSize.second;

	D3DXMatrixTranslation(&MTranslation,
		ClientSizeX* 0.5f,
		ClientSizeY *0.5f, 0.f);

	float SizeX = ClientSizeX / float(sp_TexInfo->ImageInfo.Width);
	float SizeY = ClientSizeY / float(sp_TexInfo->ImageInfo.Height);

	D3DXMatrixScaling(&MScale, SizeX*SelectPictureViewScale, SizeY*SelectPictureViewScale, 0.f);

	MWorld = MScale * MTranslation;

	auto& _GraphicDeviceRef = GraphicDevice::instance();

	_GraphicDeviceRef.GetSprite()->SetTransform(&MWorld);

	_GraphicDeviceRef.instance().GetSprite()->Draw(sp_TexInfo->pTexture, nullptr,
		&vec3(CenterX, CenterY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));

	_GraphicDeviceRef.RenderEnd(_Picture.GetSafeHwnd());

	UpdateData(FALSE);
}



void CMapTool::OnBnClickedMapSaveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto* pView = GetView();
	if (!pView)return;
	if (!pView->up_Terrain)return;

	if (CheckBoxRenderTile.GetCheck())
	{
		MessageBox(L"Render Tile Info Save !", L"SAVE", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(FALSE, this);
		pView->up_Terrain->SaveTilesCurrentStateKeyOnly(FilePath);
	}
		
	if (CheckBoxCollisionTile.GetCheck())
	{
		MessageBox(L"Collision Tile Info Save !", L"SAVE", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(FALSE, this);
		pView->_CollisionTileManager.SaveCollisionTile(FilePath);
	}

	if (CheckBoxLine.GetCheck())
	{
		MessageBox(L"Collision Line Info Save !", L"SAVE", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(FALSE, this);

		pView->_CollisionLineManager.SaveCollisionLine(FilePath);
	}

	if (CheckBoxObjectMode.GetCheck())
	{
		MessageBox(L"Object Info Save !", L"SAVE", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(FALSE, this);

		pView->_ObjectEdit.SaveCurrentStageObjInfo(FilePath);
	}
}


void CMapTool::OnBnClickedMapLoadButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto* pView = GetView();
	if (!pView)return;
	if (!pView->up_Terrain)return;

	if (CheckBoxRenderTile.GetCheck())
	{
		MessageBox(L"Render Tile Info Load !", L"LOAD", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(TRUE, this);
		pView->up_Terrain->LoadTilesCurrentStateKeyOnly(FilePath);
	}

	if (CheckBoxCollisionTile.GetCheck())
	{
		MessageBox(L"Collision Tile Info Load !", L"LOAD", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(TRUE, this);
		pView->_CollisionTileManager.LoadCollisionTile(FilePath);
	}

	if (CheckBoxLine.GetCheck())
	{
		MessageBox(L"Collision Line Info Load !", L"LOAD", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(TRUE, this);
		pView->_CollisionLineManager.LoadCollisionLine(FilePath);
	}

	if (CheckBoxObjectMode.GetCheck())
	{
		MessageBox(L"Object Info Load !", L"LOAD", MB_OK);
		std::wstring FilePath = FileHelper::GetOperationFilePath(TRUE, this);
		pView->_ObjectEdit.LoadCurrentStageObjInfo(FilePath);
	}
}

CMFCToolView *  CMapTool::GetView() const &
{
	CMainFrame*pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (!pMain)return nullptr;
	CMFCToolView*pView = dynamic_cast<CMFCToolView*>(pMain->_MainSplitter.GetPane(0, 1));
	if (!pView)return nullptr;

	return pView;
}


void CMapTool::OnMapStateRadioBtnClickEvent()
{
	UpdateData(TRUE);

	_ListBox.ResetContent();

	for (auto& Statekey_RadioBtn : MapTexStateKeyRadioBtnMap)
	{
		auto& RadioBtn = Statekey_RadioBtn.second.first;
		auto& StateKey = Statekey_RadioBtn.first;
		
		// 여기서 맵 모드 스테이트키 업데이트가 필요한 객체들에게 
		// 변경사항을 전파합니다.
		if (RadioBtn.GetCheck())
		{
			auto* pView = GetView();
			if (!pView)return;
			if (!pView->up_Terrain)return;

			GetView()->up_Terrain->CurrentTileTextureStateKey = StateKey;
			GetView()->_CollisionTileManager.CurrentStateKey = StateKey;
			GetView()->_CollisionLineManager.CurrentStateKey = StateKey;
			GetView()->_ObjectEdit.CurrentEditStage = StateKey;

			CurrentSelectRadioBtnText = StateKey;

			const auto& CurrentSelectFileSet = Statekey_RadioBtn.second.second;

			for (const auto& FileName : CurrentSelectFileSet)
			{
				_ListBox.AddString(FileName.c_str());
			}
		}
	}

	UpdateData(FALSE);
}

void CMapTool::OnMapLayerRadioBtnClickEvent()
{
	for (ELayer_Map LayerMapIDX = ELayer_Map::BACK_GROUND; LayerMapIDX != ELayer_Map::NONE; ++LayerMapIDX)
	{
		if (MapLayerSelectRadioBtnTable[LayerMapIDX].GetCheck())
			_CurrentSelectLayerMap = LayerMapIDX;
	};
}

void CMapTool::OnBnClickedCheckRenderTile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto*pView = GetView();
	if (!pView)return;

	pView->bRenderTileMode = CheckBoxRenderTile.GetCheck(); 
}


void CMapTool::OnBnClickedCheckCollisionTile()
{
	auto*pView=GetView();
	if (!pView)return;

	pView->bCollisionTileMode = CheckBoxCollisionTile.GetCheck(); 
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool::OnBnClickedCheckLine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto*pView = GetView();
	if (!pView)return;

	pView->bLineMode = CheckBoxLine.GetCheck();
}


void CMapTool::OnBnClickedCheckObjectToggle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto*pView = GetView();
	if (!pView)return;

	pView->bObjectMode= CheckBoxObjectMode.GetCheck();
}



void CMapTool::OnBnClickedCheckWallRide()
{
	auto*pView = GetView();
	if (!pView)return;

	pView->bWallRide = CheckBoxWallRide.GetCheck();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool::OnBnClickedCheckTileCanGoDown()
{
	auto*pView = GetView();
	if (!pView)return;

	pView->bTileCanGoDown = CheckBoxTileCanGoDown.GetCheck();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CMapTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	MapTexStateKeyRadioBtnMap[L"Prison"s].first.SetCheck(TRUE);
	MapLayerSelectRadioBtnTable[ELayer_Map::BACK_GROUND].SetCheck(TRUE);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
