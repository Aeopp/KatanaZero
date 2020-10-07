#pragma once
#include "afxwin.h"

// CItemTool ��ȭ �����Դϴ�.

class CObjectTool : public CDialog
{
	DECLARE_DYNAMIC(CObjectTool)

public:
	CObjectTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CObjectTool();

	//virtual void OnFinalRelease();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ITEMTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	//DECLARE_DISPATCH_MAP()
	//DECLARE_INTERFACE_MAP()
public:
	afx_msg void OnLbnSelchangeItemList1();
	CListBox _ListBox;
	CStatic _Picture;
	int _DrawID{ 0 };
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void HorizontalScroll();
public:
private:
	class CMFCToolView*   GetView()const&;
};
