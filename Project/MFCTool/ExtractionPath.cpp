// ExtractionPath.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCTool.h"
#include "ExtractionPath.h"
#include "afxdialogex.h"
#include <fstream>
#include <sstream>
#include "TupleGlobalHelper.h"

// CExtractionPath 대화 상자

IMPLEMENT_DYNAMIC(CExtractionPath, CDialog)

CExtractionPath::CExtractionPath(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_EXTRACTIONPATH, pParent)
{

}

CExtractionPath::~CExtractionPath()
{
}

void CExtractionPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, _ListDirInfo.first);
}


BEGIN_MESSAGE_MAP(CExtractionPath, CDialog)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTONSAVE, &CExtractionPath::OnBnClickedButtonsave)
	ON_BN_CLICKED(IDC_BUTTONLOAD, &CExtractionPath::OnBnClickedButtonload)
	ON_BN_CLICKED(IDC_BUTTONCLEAR, &CExtractionPath::OnBnClickedButtonclear)
END_MESSAGE_MAP()
// CExtractionPath 메시지 처리기
void CExtractionPath::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	TCHAR FullPath[MAX_PATH] = L"";
	
	int32_t  Size = DragQueryFile(hDropInfo, -1, nullptr, 0);


	for (int32_t i = 0; i < Size; ++i)
	{
		DragQueryFile(hDropInfo, i, FullPath, MAX_PATH);
		// 작업을 싹 할것
		
		auto&& FileInfoExtractVec = FileHelper::DirectoryInfoExtraction(FullPath);

		_ListDirInfo.second.insert(std::end(_ListDirInfo.second),
			std::make_move_iterator(std::begin(FileInfoExtractVec)),  
			std::make_move_iterator(std::end(FileInfoExtractVec)));
	}
	_ListDirInfo.first.ResetContent();
	std::wstringstream wss;

	// 여기서 리스트박스 정보 갱신을 해주세요. 
	for (auto& _TexPath : _ListDirInfo.second)
	{
		wss << _TexPath.RelativePath << L"|" << _TexPath.ObjectKey << L"|"
			<< _TexPath.StateKey << L"|" << _TexPath.Count;

		_ListDirInfo.first.AddString(wss.str().c_str());

		wss.clear();
		wss.str(L"");
	}
	///////////
	HorizontalScroll();
	UpdateData(FALSE);
	CDialog::OnDropFiles(hDropInfo);
}

void CExtractionPath::OnBnClickedButtonsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//파일 입출력을 하기 위한 객체 생성! 
	using  namespace TupleGlobalHelper;
	 
	wofstream FOut;
	FOut.open("../Data/PathInfo.txt");
	if (!FOut.fail())
	{
		for (auto& _TexPath : _ListDirInfo.second)
		{
			FOut << _TexPath.RefTuple();
		}
	}
	WinExec("notepad.exe ../Data/PathInfo.txt", SW_SHOW);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CExtractionPath::OnBnClickedButtonload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	using namespace TupleGlobalHelper;

	UpdateData(TRUE);
	wifstream FIn;
	FIn.open("../Data/PathInfo.txt");
	
	std::wstringstream wss;
	if (!FIn.fail())
	{
		_ListDirInfo.first.ResetContent();
		_ListDirInfo.second.clear();
		while (true)
		{
			if (FIn.eof())
				break;

			TexPath _TexPath;
			auto&& DataTuple = _TexPath.RefTuple();
			FIn >> DataTuple;
			if (!_TexPath.Count)continue;

			wss << _TexPath.RelativePath << L"|" << _TexPath.ObjectKey << L"|"
				<< _TexPath.StateKey << L"|" << _TexPath.Count;

			_ListDirInfo.first.AddString(wss.str().c_str());
			_ListDirInfo.second.emplace_back(std::move(_TexPath));
			wss.clear();
			wss.str(L"");
		}
		FIn.close();
	}
	HorizontalScroll();
	UpdateData(FALSE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CExtractionPath::HorizontalScroll() & noexcept
{
	CString _Name;
	CSize _Size;
	int Extent = 0;
	CDC* _DC = _ListDirInfo.first.GetDC();
	for (int i = 0; i < _ListDirInfo.first.GetCount(); ++i)
	{
		_ListDirInfo.first.GetText(i, _Name);
		_Size = _DC->GetTextExtent(_Name);

		if (_Size.cx > Extent)
			Extent = _Size.cx;
	}
	_ListDirInfo.first.ReleaseDC(_DC);

	if (Extent > _ListDirInfo.first.GetHorizontalExtent())
	{
		_ListDirInfo.first.SetHorizontalExtent(Extent);
	}
}





void CExtractionPath::OnBnClickedButtonclear()
{
	if (IDYES == MessageBox(L"정말로 파일 텍스쳐 정보를 초기화 할까요 ? ", L"FileDirInfoClear", MB_YESNO))
	{
		_ListDirInfo.first.ResetContent();
		_ListDirInfo.second.clear();

	 }
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
