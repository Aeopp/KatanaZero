#pragma once
#include "afxwin.h"


// CMapTool ��ȭ �����Դϴ�.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void HorizontalScroll();
	CListBox _ListBox;
	CStatic _Picture;

	int _DrawID{ 0 };
	ELayer_Map _CurrentSelectLayerMap{ ELayer_Map::BACK_GROUND }; 

	afx_msg void OnLbnSelchangePicture();
	afx_msg void OnBnClickedMapSaveButton();
	afx_msg void OnBnClickedMapLoadButton();
private:
	class CMFCToolView*   GetView()const&;
public:
	std::map<std::wstring, CButton > MapTexStateKeyRadioBtnMap;
	afx_msg void OnBnClickedRadioMapMode();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio6();
};
