// MyForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, ButtonDebug);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyForm::OnBnClickedUnitTool)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyForm::OnBnClickedMapTool)
	ON_BN_CLICKED(IDC_CHECK1, &CMyForm::OnBnClickedCheckDebug)
	ON_BN_CLICKED(IDC_BUTTON7, &CMyForm::OnBnClickedItemEdit)
	ON_BN_CLICKED(IDC_BUTTONPATHEDIT, &CMyForm::OnBnClickedButtonpathedit)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.


void CMyForm::OnBnClickedUnitTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(_UnitTool.GetSafeHwnd() == nullptr)
		_UnitTool.Create(IDD_UNITTOOL);

	_UnitTool.ShowWindow(SW_SHOW); 
}


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	_Font.CreatePointFont(150, L"메이플스토리");
	GetDlgItem(IDC_BUTTON2)->SetFont(&_Font);

	global::bDebug = ButtonDebug.GetCheck();
}


void CMyForm::OnBnClickedMapTool()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == _MapTool.GetSafeHwnd())
		_MapTool.Create(IDD_MAPTOOL);

	_MapTool.ShowWindow(SW_SHOW);
}

void CMyForm::OnBnClickedItemEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == _ObjectTool.GetSafeHwnd())
		_ObjectTool.Create(IDD_ITEMTOOL);

	_ObjectTool.ShowWindow(SW_SHOW);

}


void CMyForm::OnBnClickedCheckDebug()
{
	global::bDebug = ButtonDebug.GetCheck();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMyForm::OnBnClickedButtonpathedit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == _ExtractionPath.GetSafeHwnd())
		_ExtractionPath.Create(IDD_EXTRACTIONPATH);

	_ExtractionPath.ShowWindow(SW_SHOW);
}
