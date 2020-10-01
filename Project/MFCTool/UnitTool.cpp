// UnitTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "UnitTool.h"
#include "afxdialogex.h"
#include <fstream>
#include <ostream>
#include <istream>
#include <numeric>


// CUnitTool 대화 상자입니다.

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


// CUnitTool 메시지 처리기입니다.

void CUnitTool::OnBnClickedChangeString()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE); 
	ChangeSecond = L"OnBnClickedChangeString Test String ";
	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedAddButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	_Radios[0].SetCheck(TRUE); 
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CUnitTool::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

// 여기에 세이브 버튼을 클릭하였을시 이벤트를 정의합니다.
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
			int32_t Select = MessageBox(L"누락된 데이터 발견 ! 계속 진행 할까요?", L"Warning!", MB_YESNO);

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

// 여기에 로드버튼을 클릭하였을시 이벤트를 정의합니다.
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
