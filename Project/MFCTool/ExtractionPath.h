#pragma once


// CExtractionPath 대화 상자

class CExtractionPath : public CDialog
{
	DECLARE_DYNAMIC(CExtractionPath)

public:
	CExtractionPath(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CExtractionPath();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CExtractionPath };
#endif
public:
	std::pair<CListBox, std::vector<TexPath>> _ListDirInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonsave();
	afx_msg void OnBnClickedButtonload();
private:
	void HorizontalScroll()& noexcept ;
public:
	afx_msg void OnBnClickedButtonclear();
};
