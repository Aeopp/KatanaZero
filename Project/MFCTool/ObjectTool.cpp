// ItemTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "ObjectTool.h"
#include "afxdialogex.h"
#include "MFCToolView.h"
#include "MainFrm.h"
#include "Terrain.h"

// CItemTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectTool, CDialog)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ITEMTOOL, pParent)
{
	_DrawID = 0;
}

CObjectTool::~CObjectTool()
{
}


void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, _ListBox);
	DDX_Control(pDX, IDC_PICTURE, _Picture);
}


BEGIN_MESSAGE_MAP(CObjectTool, CDialog)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST1, &CObjectTool::OnLbnSelchangeItemList)
END_MESSAGE_MAP()


// CItemTool 메시지 처리기입니다.
void CObjectTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	_ListBox.ResetContent();

	TCHAR szFilePath[MAX_PATH] = L"";
	TCHAR szFileName[MAX_PATH] = L"";
	CString strReletivePath = L"";
	int iSize = DragQueryFile(hDropInfo, -1, nullptr, 0);
	CString strFileNameAndExtant = L"";
	for (int i = 0; i < iSize; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);
		strReletivePath = FileHelper::ConvertReletivePath(szFilePath);
		strFileNameAndExtant = PathFindFileName(strReletivePath);
		lstrcpy(szFileName, strFileNameAndExtant.GetString());
		PathRemoveExtension(szFileName);
		_ListBox.AddString(szFileName);
	}

	HorizontalScroll();
	UpdateData(FALSE);
	CDialog::OnDropFiles(hDropInfo);
}


void CObjectTool::HorizontalScroll()
{
	CString Name;
	CSize Size;
	int CX = 0;
	CDC* pDC = _ListBox.GetDC();

	for (int i = 0; i < _ListBox.GetCount(); ++i)
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


void CObjectTool::OnLbnSelchangeItemList()
{

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		Get_TexInfo(L"EditObject",L"PlacePicture", _DrawID);

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


CMFCToolView *  CObjectTool::GetView() const &
{
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (!pMain)return nullptr;
	CMFCToolView*pView = dynamic_cast<CMFCToolView*>(pMain->_MainSplitter.GetPane(0, 1));
	if (!pView)return nullptr;

	return pView;
}

