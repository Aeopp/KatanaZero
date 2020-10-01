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
	MapTexStateKeyRadioBtnMap[L"Prison"s];
	MapTexStateKeyRadioBtnMap[L"Mansion"s];
	MapTexStateKeyRadioBtnMap[L"Factory"s];
	MapTexStateKeyRadioBtnMap[L"Chinatown"s];
	MapTexStateKeyRadioBtnMap[L"Boss"s];
}

CMapTool::~CMapTool()
{
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, _ListBox);
	DDX_Control(pDX, IDC_PICTURE, _Picture);

	DDX_Control(pDX, IDC_RADIO2, MapTexStateKeyRadioBtnMap[L"Prison"s]);
	DDX_Control(pDX, IDC_RADIO3, MapTexStateKeyRadioBtnMap[L"Factory"s]);
	DDX_Control(pDX, IDC_RADIO1, MapTexStateKeyRadioBtnMap[L"Mansion"s]);
	DDX_Control(pDX, IDC_RADIO4, MapTexStateKeyRadioBtnMap[L"Chinatown"s]);
	DDX_Control(pDX, IDC_RADIO6, MapTexStateKeyRadioBtnMap[L"Boss"s]);
	DDX_Control(pDX, IDC_CHECK3, CheckBoxRenderTile);
	DDX_Control(pDX, IDC_CHECK1, CheckBoxCollisionTile);
	DDX_Control(pDX, IDC_CHECK2, CheckBoxLine);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnLbnSelchangePicture)
	ON_BN_CLICKED(IDC_BUTTON1, &CMapTool::OnBnClickedMapSaveButton)
	ON_BN_CLICKED(IDC_BUTTON2, &CMapTool::OnBnClickedMapLoadButton)
	ON_BN_CLICKED(IDC_RADIO2, &CMapTool::OnBnClickedRadioMapMode)
	ON_BN_CLICKED(IDC_RADIO3, &CMapTool::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO1, &CMapTool::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO4, &CMapTool::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO6, &CMapTool::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_CHECK3, &CMapTool::OnBnClickedCheckRenderTile)
	ON_BN_CLICKED(IDC_CHECK1, &CMapTool::OnBnClickedCheckCollisionTile)
	ON_BN_CLICKED(IDC_CHECK2, &CMapTool::OnBnClickedCheckLine)
END_MESSAGE_MAP()

// CMapTool 메시지 처리기입니다.
// 해당 함수 호출 전 파일은 로딩이 끝난 상태여야 합니다.
void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE); 
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
	int i = 0; 
	for (; i < CurrentSelectName.GetLength(); ++i)
	{
		if(isdigit(CurrentSelectName[i]))
			break;
	}
	// 해당 작업 이후 파일명에서 숫자만 남습니다.
	CurrentSelectName.Delete(0,i);

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
		ClientSizeY *0.5f,0.f);

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

	std::wstring FilePath = FileHelper::GetOperationFilePath(FALSE, this);

	if (!pView->up_Terrain)return;

	if (CheckBoxRenderTile.GetCheck())
	{
		MessageBox(L"Render Tile Info Save !", L"SAVE", MB_OK);
		pView->up_Terrain->SaveTilesCurrentStateKeyOnly(FilePath);
	}
		
	if (CheckBoxCollisionTile.GetCheck())
	{
		MessageBox(L"Collision Tile Info Save !", L"SAVE", MB_OK);
		pView->_CollisionTileManager.SaveCollisionTile(FilePath);
	}
		
}


void CMapTool::OnBnClickedMapLoadButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto* pView = GetView();
	if (!pView)return;

	std::wstring FilePath = FileHelper::GetOperationFilePath(TRUE, this);

	if (!pView->up_Terrain)return;

	if (CheckBoxRenderTile.GetCheck())
	{
		MessageBox(L"Render Tile Info Load !", L"LOAD", MB_OK);
		pView->up_Terrain->LoadTilesCurrentStateKeyOnly(FilePath);
	}

	if (CheckBoxCollisionTile.GetCheck())
	{
		MessageBox(L"Collision Tile Info Load !", L"LOAD", MB_OK);
		pView->_CollisionTileManager.LoadCollisionTile(FilePath);
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
	for (auto& Statekey_RadioBtn : MapTexStateKeyRadioBtnMap)
	{
		auto& RadioBtn = Statekey_RadioBtn.second;
		auto& StateKey = Statekey_RadioBtn.first;

		// 여기서 맵 모드 스테이트키 업데이트가 필요한 객체들에게 
		// 변경사항을 전파합니다.
		if (RadioBtn.GetCheck())
		{
			auto* pView = GetView();
			if (!pView)return;
			if (!pView->up_Terrain)return;

			GetView()->up_Terrain->CurrentTileTextureStateKey = StateKey;
		}
			
	}
}

void CMapTool::OnBnClickedRadioMapMode()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool::OnBnClickedRadio3()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool::OnBnClickedRadio1()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool::OnBnClickedRadio4()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool::OnBnClickedRadio6()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
