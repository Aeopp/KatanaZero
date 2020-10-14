#include "stdafx.h"
#include "FileInfo.h"

CString FileHelper::ConvertReletivePath(CString FullPath)
{
	TCHAR ReletivePath[MAX_PATH] = L""; 
	TCHAR CurrentDirectory[MAX_PATH] = L"";
	
	GetCurrentDirectory(MAX_PATH, CurrentDirectory);
	PathRelativePathTo(ReletivePath, CurrentDirectory,
	FILE_ATTRIBUTE_DIRECTORY, FullPath.GetString(),FILE_ATTRIBUTE_DIRECTORY); 
	return CString(ReletivePath);
}

std::vector<TexPath> FileHelper::DirectoryInfoExtraction(const std::wstring& FullPath)noexcept
{
	CFileFind Find;

	wstring Path = FullPath + L"\\*.*";

	std::vector<TexPath> TexPathContainer;

	Find.FindFile(Path.c_str());
	BOOL bContinue = TRUE;
	while (bContinue)
	{
		bContinue = Find.FindNextFile();
		if (Find.IsDots())
			continue;
		else if (Find.IsDirectory())
		{
			std::vector<TexPath>&& Inner_TexPathContainer = FileHelper::DirectoryInfoExtraction(Find.GetFilePath().GetString());
			TexPathContainer.insert
				(std::end(TexPathContainer),
				std::make_move_iterator(std::begin(Inner_TexPathContainer)),
				std::make_move_iterator(std::end(Inner_TexPathContainer)));

		/*	std::move_backward(std::begin(Inner_TexPathContainer),std::end(Inner_TexPathContainer),
			std::end(TexPathContainer));*/
		}
		else
		{
			if (Find.IsSystem())
				continue;
			// 파일 접근 성공
			TexPath _TexPath; 
			TCHAR PathBuf[MAX_PATH] = L"";
			// 파일의 갯수부터 파악. 
			lstrcpy(PathBuf, Find.GetFilePath().GetString());
			PathRemoveFileSpec(PathBuf);
			TCHAR ObjectKey_StateKey_Slice[MAX_PATH] = L"";
			lstrcpy(ObjectKey_StateKey_Slice, PathBuf);
			_TexPath.Count = CalcDirectoryFileCount(PathBuf);

			wstring FileName = Find.GetFileTitle().GetString();
			FileName = FileName.substr(0, FileName.length() - 1) + L"%d.png";
			PathCombine(PathBuf, PathBuf, FileName.c_str());
			_TexPath.RelativePath = FileHelper::ConvertReletivePath(PathBuf);
			_TexPath.StateKey = PathFindFileName(ObjectKey_StateKey_Slice);

			PathRemoveFileSpec(ObjectKey_StateKey_Slice);
			_TexPath.ObjectKey = PathFindFileName(ObjectKey_StateKey_Slice);
			TexPathContainer.emplace_back(std::move(_TexPath));
			bContinue = FALSE;
		}
	}

	return TexPathContainer;
}

uint32_t FileHelper::CalcDirectoryFileCount(const std::wstring& FullPath)
{
	CFileFind Find;
	wstring Path = FullPath + L"\\*.png";
	BOOL bContinue = Find.FindFile(Path.c_str());
	uint32_t Count = 0;
	while (bContinue)
	{
		bContinue = Find.FindNextFile();
		if (Find.IsDots() || Find.IsSystem())
			continue;
		++Count;
	}
	return Count;
}

std::wstring FileHelper::GetOperationFilePath(bool FileModeFlag,CWnd*const MFCWndClassPtr)
{
	std::wstring FilePath;

	if (!MFCWndClassPtr)
		return FilePath;

	CFileDialog Dlg(FileModeFlag,// 모드 지정 -true라면 열기모드, false라면 저장모드
		L"txt",
		L"*.txt",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data File(*.txt) |*.txt||", MFCWndClassPtr);

	TCHAR Path[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, Path);
	PathRemoveFileSpec(Path);
	lstrcat(Path, L"\\Data");
	Dlg.m_ofn.lpstrInitialDir = Path;

	if (IDOK == Dlg.DoModal())
	{
		FilePath = Dlg.GetPathName();
	}

	return FilePath;
}





