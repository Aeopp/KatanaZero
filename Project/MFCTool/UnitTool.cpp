// UnitTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "UnitTool.h"
#include "afxdialogex.h"
#include <fstream>
#include <ostream>
#include <istream>
#include <numeric>


// CUnitTool ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)

CUnitTool::CUnitTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UNITTOOL, pParent)
	, ChangeFirst(_T(""))
	, ChangeSecond(_T(""))
	, Name(_T(""))
	, Attack(0)
	, Defense(0)
	, FindName(_T(""))
{

}

CUnitTool::~CUnitTool()
{
}

void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, ChangeFirst);
	DDX_Text(pDX, IDC_EDIT2, ChangeSecond);
	DDX_Text(pDX, IDC_EDIT3, Name);
	DDX_Text(pDX, IDC_EDIT4, Attack);
	DDX_Text(pDX, IDC_EDIT5, Defense);
	DDX_Control(pDX, IDC_LIST1, _ListBox);

	DDX_Control(pDX, IDC_RADIO1, _Radios[0]);
	DDX_Control(pDX, IDC_RADIO2, _Radios[1]);
	DDX_Control(pDX, IDC_RADIO3, _Radios[2]);

	DDX_Control(pDX, IDC_CHECK1, _CheckBoxs[0]);
	DDX_Control(pDX, IDC_CHECK2, _CheckBoxs[1]);
	DDX_Control(pDX, IDC_CHECK3, _CheckBoxs[2]);

	DDX_Text(pDX, IDC_EDIT6, FindName);
}


BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedChangeString)
	ON_BN_CLICKED(IDC_BUTTON2, &CUnitTool::OnBnClickedAddButton)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUnitTool::OnLbnSelchangeClassName)
	ON_BN_CLICKED(IDC_BUTTON3, &CUnitTool::OnBnClickedDelete)
	ON_EN_CHANGE(IDC_EDIT6, &CUnitTool::OnEnChangeFindName)
	ON_BN_CLICKED(IDC_BUTTON4, &CUnitTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON5, &CUnitTool::OnBnClickedLoad)
END_MESSAGE_MAP()


// CUnitTool �޽��� ó�����Դϴ�.

void CUnitTool::OnBnClickedChangeString()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData(TRUE); 
	ChangeSecond = L"OnBnClickedChangeString Test String ";
	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedAddButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE); 

	auto iter_find = _UnitInfoMap.find(Name);
	if (_UnitInfoMap.end() != iter_find)
		return; 

	std::shared_ptr<UnitInfo> sp_Unit = std::make_shared<UnitInfo>();
	if (!sp_Unit)return;

	sp_Unit->Name = Name; 
	sp_Unit->Attack = Attack;
	sp_Unit->Defense = Defense;
	sp_Unit->Item = 0;

	int i = 0;
	for (; i < 3; ++i)
	{
		if(_Radios[i].GetCheck())
			break;
	}

	sp_Unit->Job = i;

	if (_CheckBoxs[0].GetCheck())
		sp_Unit->Item |= Sword;
	if (_CheckBoxs[1].GetCheck())
		sp_Unit->Item |= Shoes;
	if (_CheckBoxs[2].GetCheck())
		sp_Unit->Item |= Shocks;

	_ListBox.AddString(Name);
	_UnitInfoMap.emplace(sp_Unit->Name, std::move(sp_Unit));
	UpdateData(FALSE); 
}


void CUnitTool::OnLbnSelchangeClassName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE); 
	for (int i = 0; i < 3; ++i)
	{
		_Radios[i].SetCheck(FALSE);
		_CheckBoxs[i].SetCheck(FALSE); 
	}
	int Index = _ListBox.GetCurSel();
	if (-1 == Index)
		return; 
	CString FindName;
	_ListBox.GetText(Index, FindName);

	auto iter_find = _UnitInfoMap.find(FindName);
	if (_UnitInfoMap.end() == iter_find)
		return; 

	Name = iter_find->second->Name; 
	Attack = iter_find->second->Attack; 
	Defense = iter_find->second->Defense; 
	_Radios[iter_find->second->Job].SetCheck(TRUE);
	
	if (iter_find->second->Item & Sword)
		_CheckBoxs[0].SetCheck(TRUE);
	if (iter_find->second->Item & Shoes)
		_CheckBoxs[1].SetCheck(TRUE);
	if (iter_find->second->Item & Shocks)
		_CheckBoxs[2].SetCheck(TRUE);

	UpdateData(FALSE); 
}


BOOL CUnitTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	_Radios[0].SetCheck(TRUE); 
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CUnitTool::OnBnClickedDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE); 
	int Index = _ListBox.GetCurSel(); 
	if (LB_ERR == Index)
		return; 
	CString FindName = L""; 
	_ListBox.GetText(Index, FindName); 
	auto iter_find = _UnitInfoMap.find(FindName); 
	if (iter_find == _UnitInfoMap.end())
		return; 

	_UnitInfoMap.erase(iter_find); 
	_ListBox.DeleteString(Index); 
	UpdateData(FALSE); 
}


void CUnitTool::OnEnChangeFindName()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	auto iter_Find = _UnitInfoMap.find(FindName);
	if (iter_Find == _UnitInfoMap.end())
		return; 

	int iIndex = _ListBox.FindString(0, FindName);
	if (LB_ERR == iIndex)
		return; 
	_ListBox.SetCurSel(iIndex);
	Name = iter_Find->second->Name; 
	Attack = iter_Find->second->Attack; 
	Defense = iter_Find->second->Defense; 

	for (int i = 0 ; i < 3; ++i)
	{
		_Radios[i].SetCheck(FALSE); 
		_CheckBoxs[i].SetCheck(FALSE); 
	}
	_Radios[iter_Find->second->Job].SetCheck(TRUE);
	//0000 0100
	//0000 0001
	//0000 0010
	//0000 0100
	if (iter_Find->second->Item & Sword)
		_CheckBoxs[0].SetCheck(TRUE);
	if (iter_Find->second->Item & Shoes)
		_CheckBoxs[1].SetCheck(TRUE);
	if (iter_Find->second->Item & Shocks)
		_CheckBoxs[2].SetCheck(TRUE);
	UpdateData(FALSE); 
}

// ���⿡ ���̺� ��ư�� Ŭ���Ͽ����� �̺�Ʈ�� �����մϴ�.
void CUnitTool::OnBnClickedSave()
{
	std::wstring FilePath = FileHelper::GetOperationFilePath(FALSE, this);

	std::wofstream file_Input(FilePath);

	size_t _InfoSize =  _UnitInfoMap.size();

	file_Input << _InfoSize << std::endl;

	for (auto& Name_spUnitInfo : _UnitInfoMap)
	{
		if (!Name_spUnitInfo.second)
		{
			int32_t Select = MessageBox(L"������ ������ �߰� ! ��� ���� �ұ��?", L"Warning!", MB_YESNO);

			switch (Select)
			{
			case IDYES:
				continue; 
				break;
			case IDNO:
				return;
				break;
			default:
				break;
			}
		}

		file_Input << *Name_spUnitInfo.second;
	}
}

// ���⿡ �ε��ư�� Ŭ���Ͽ����� �̺�Ʈ�� �����մϴ�.
void CUnitTool::OnBnClickedLoad()
{
	std::wstring FilePath = FileHelper::GetOperationFilePath(TRUE, this);

	std::wifstream file_output(FilePath);
	size_t _InfoSize{};
	file_output >> _InfoSize;

	for (size_t i = 0; i < _InfoSize; ++i)
	{
		auto spUnitInfo = std::make_shared<UnitInfo>();

		file_output >> *spUnitInfo;

		std::wstring InsertNamekey = spUnitInfo->Name;

		_ListBox.AddString(InsertNamekey.c_str());

		_UnitInfoMap.emplace(InsertNamekey.c_str(),std::move(spUnitInfo));
	}
}
