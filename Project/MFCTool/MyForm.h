#pragma once



// CMyForm 폼 뷰입니다.
#include "UnitTool.h"
#include "MapTool.h"
#include "ObjectTool.h"
#include "ExtractionPath.h"

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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


