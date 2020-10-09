// MyForm.cpp : ���� �����Դϴ�.
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
END_MESSAGE_MAP()


// CMyForm �����Դϴ�.

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


// CMyForm �޽��� ó�����Դϴ�.


void CMyForm::OnBnClickedUnitTool()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(_UnitTool.GetSafeHwnd() == nullptr)
		_UnitTool.Create(IDD_UNITTOOL);

	_UnitTool.ShowWindow(SW_SHOW); 
}


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	_Font.CreatePointFont(150, L"�����ý��丮");
	GetDlgItem(IDC_BUTTON2)->SetFont(&_Font);

	global::bDebug = ButtonDebug.GetCheck();
}


void CMyForm::OnBnClickedMapTool()
{
	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == _MapTool.GetSafeHwnd())
		_MapTool.Create(IDD_MAPTOOL);

	_MapTool.ShowWindow(SW_SHOW);
}

void CMyForm::OnBnClickedItemEdit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (nullptr == _ObjectTool.GetSafeHwnd())
		_ObjectTool.Create(IDD_ITEMTOOL);

	_ObjectTool.ShowWindow(SW_SHOW);

}


void CMyForm::OnBnClickedCheckDebug()
{
	global::bDebug = ButtonDebug.GetCheck();
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

