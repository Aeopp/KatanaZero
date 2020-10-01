#pragma once
#include "afxwin.h"


// CUnitTool 대화 상자입니다.

class CUnitTool : public CDialog
{
	DECLARE_DYNAMIC(CUnitTool)

public:
	CUnitTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUnitTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNITTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString ChangeFirst;
	CString ChangeSecond;
	
	// UnitData
	CString Name;
	int Attack;
	int Defense;
	CListBox _ListBox;

	std::array<CButton ,3>_Radios;
	std::array<CButton, 3>_CheckBoxs;

	map<CString, std::shared_ptr<UnitInfo>> _UnitInfoMap;
	CString FindName;

	afx_msg void OnBnClickedAddButton();
	afx_msg void OnLbnSelchangeClassName();
	afx_msg void OnBnClickedChangeString();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnEnChangeFindName();
	
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
};
