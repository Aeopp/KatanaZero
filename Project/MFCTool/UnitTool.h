#pragma once
#include "afxwin.h"


// CUnitTool ��ȭ �����Դϴ�.

class CUnitTool : public CDialog
{
	DECLARE_DYNAMIC(CUnitTool)

public:
	CUnitTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CUnitTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNITTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
