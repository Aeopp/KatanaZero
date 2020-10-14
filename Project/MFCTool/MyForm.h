#pragma once



// CMyForm �� ���Դϴ�.
#include "UnitTool.h"
#include "MapTool.h"
#include "ObjectTool.h"
#include "ExtractionPath.h"

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMyForm();
public:
	CUnitTool _UnitTool; 
	CMapTool _MapTool; 
	CObjectTool _ObjectTool;
	CExtractionPath _ExtractionPath;
	CFont _Font; 
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUnitTool();
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedMapTool();
	afx_msg void OnBnClickedItemEdit();
	CButton ButtonDebug;
	afx_msg void OnBnClickedCheckDebug();
	
	afx_msg void OnBnClickedButtonpathedit();
};


