#pragma once
#include "afxwin.h"
#include <set>
// CMapTool 대화 상자입니다.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void HorizontalScroll();
	CListBox _ListBox;
	CStatic _Picture;

	std::wstring CurrentSelectRadioBtnText;
	int _DrawID{ 0 };
	ELayer_Map _CurrentSelectLayerMap{ ELayer_Map::BACK_GROUND }; 

	afx_msg void OnLbnSelchangePicture();
	afx_msg void OnBnClickedMapSaveButton();
	afx_msg void OnBnClickedMapLoadButton();
private:
	class CMFCToolView*   GetView()const&;
public:
	std::map<std::wstring,std::pair<CButton,std::set<std::wstring>>> MapTexStateKeyRadioBtnMap;

	void OnMapStateRadioBtnClickEvent();

	afx_msg void OnBnClickedRadioMapMode();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio7();

	CButton CheckBoxRenderTile;
	CButton CheckBoxCollisionTile;
	CButton CheckBoxLine;
	afx_msg void OnBnClickedCheckRenderTile();
	afx_msg void OnBnClickedCheckCollisionTile();
	afx_msg void OnBnClickedCheckLine();
	afx_msg void OnBnClickedCheckObjectToggle();
	CButton CheckBoxObjectMode;
};
