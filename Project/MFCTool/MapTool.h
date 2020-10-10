#pragma once
#include "afxwin.h"
#include <set>
#include <array>
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
	std::array<CButton, ELayer_Map::NONE> MapLayerSelectRadioBtnTable;

	afx_msg void OnMapStateRadioBtnClickEvent();
	afx_msg void OnMapLayerRadioBtnClickEvent();

	CButton CheckBoxRenderTile;
	CButton CheckBoxCollisionTile;
	CButton CheckBoxLine;
	afx_msg void OnBnClickedCheckRenderTile();
	afx_msg void OnBnClickedCheckCollisionTile();
	afx_msg void OnBnClickedCheckLine();
	afx_msg void OnBnClickedCheckObjectToggle();
	CButton CheckBoxObjectMode;
};
