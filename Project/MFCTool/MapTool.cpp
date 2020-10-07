// MapTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "MFCToolView.h"
#include "Terrain.h"

// CMapTool ��ȭ �����Դϴ�.

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
	DDX_Control(pDX, IDC_RADIO3, MapTexStateKeyRadioBtnMap[L"Factory"s].first);
	DDX_Control(pDX, IDC_RADIO1, MapTexStateKeyRadioBtnMap[L"Mansion"s].first);
	DDX_Control(pDX, IDC_RADIO4, MapTexStateKeyRadioBtnMap[L"Chinatown"s].first);
	DDX_Control(pDX, IDC_RADIO6, MapTexStateKeyRadioBtnMap[L"Boss"s].first);
	DDX_Control(pDX, IDC_RADIO5, MapTexStateKeyRadioBtnMap[L"Stage1"s].first);
	DDX_Control(pDX, IDC_RADIO7, MapTexStateKeyRadioBtnMap[L"Stage2"s].first);
	DDX_Control(pDX, IDC_CHECK3, CheckBoxRenderTile);
	DDX_Control(pDX, IDC_CHECK1, CheckBoxCollisionTile);
	DDX_Control(pDX, IDC_CHECK2, CheckBoxLine);
	DDX_Control(pDX, IDC_CHECK4, CheckBoxObjectMode);
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
	ON_BN_CLICKED(IDC_RADIO5, &CMapTool::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO7, &CMapTool::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_CHECK3, &CMapTool::OnBnClickedCheckRenderTile)
	ON_BN_CLICKED(IDC_CHECK1, &CMapTool::OnBnClickedCheckCollisionTile)
	ON_BN_CLICKED(IDC_CHECK2, &CMapTool::OnBnClickedCheckLine)
	ON_BN_CLICKED(IDC_CHECK4, &CMapTool::OnBnClickedCheckObjectToggle)
END_MESSAGE_MAP()

// CMapTool �޽��� ó�����Դϴ�.
// �ش� �Լ� ȣ�� �� ������ �ε��� ���� ���¿��� �մϴ�.
void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	UpdateData(TRUE);
	// ����Ʈ �ڽ��� �ʱ�ȭ �մϴ�. 
	_ListBox.ResetContent();
	//////////
	
	// ���� ���õ� ���� ��ư�� ���ڿ��� ã���ϴ�.
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

// TODO :: ���⿡ Ÿ�� ���� ������ ���� �̴Ϻ��� �׸��� �ڵ带 �ۼ����ּ���.
// �ش� �Լ��� ȣ�� Ÿ�̹��� ����Ʈ �ڽ��� Ÿ���� �����ϴ� �����Դϴ�.
void CMapTool::OnLbnSelchangePicture()
{
	constexpr float SelectPictureViewScale = 0.9f;

	auto* pView = GetView();
	if (!pView)return;
	if (!pView->up_Terrain)return;

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	CString CurrentSelectName = L"";
	// �����ε������� �����ϴ� ����Ʈ �ڽ��� ���� ������ �ε����Դϴ� (���ϸ�� ȥ������ ������..)
	int32_t SelectFileIndex = _ListBox.GetCurSel();

	if (LB_ERR == SelectFileIndex)
		return;

	// ����Ʈ�ڽ��� ���缱�� �ε����κ��� ���ڿ��� ���ɴϴ�.
	_ListBox.GetText(SelectFileIndex, CurrentSelectName);

	// ���ϸ��� ���ڸ� ����� ���� �����մϴ�.
	int32_t i = CurrentSelectName.GetLength() - 1;
	for (; i >= 0; --i)
	{
		if (false == isdigit(CurrentSelectName[i]))
			break;
	}
	// �ش� �۾� ���� ���ϸ��� ���ڸ� �����ϴ�.
	CurrentSelectName.Delete(0, i+1);

	// ���� ���ڿ��� ���� ������ ��ȯ�մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		
		// ���⼭ �� ��� ������ƮŰ ������Ʈ�� �ʿ��� ��ü�鿡�� 
		// ��������� �����մϴ�.
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

void CMapTool::OnBnClickedRadioMapMode()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMapTool::OnBnClickedRadio3()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMapTool::OnBnClickedRadio1()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMapTool::OnBnClickedRadio4()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMapTool::OnBnClickedRadio6()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


//Stage1
void CMapTool::OnBnClickedRadio5()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


//Stage2
void CMapTool::OnBnClickedRadio7()
{
	OnMapStateRadioBtnClickEvent();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMapTool::OnBnClickedCheckRenderTile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	auto*pView = GetView();
	if (!pView)return;

	pView->bRenderTileMode = CheckBoxRenderTile.GetCheck(); 
}


void CMapTool::OnBnClickedCheckCollisionTile()
{
	auto*pView=GetView();
	if (!pView)return;

	pView->bCollisionTileMode = CheckBoxCollisionTile.GetCheck(); 
	

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMapTool::OnBnClickedCheckLine()
{

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	auto*pView = GetView();
	if (!pView)return;

	pView->bLineMode = CheckBoxLine.GetCheck();
}


void CMapTool::OnBnClickedCheckObjectToggle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	auto*pView = GetView();
	if (!pView)return;

	pView->bObjectMode= CheckBoxObjectMode.GetCheck();
}
