#include "stdafx.h"
#include "FileInfo.h"

CString FileHelper::ConvertReletivePath(CString FullPath)
{
	TCHAR ReletivePath[MAX_PATH] = L""; 
	TCHAR CurrentDirectory[MAX_PATH] = L"";
	
	GetCurrentDirectory(MAX_PATH, CurrentDirectory);
	PathRelativePathTo(ReletivePath, CurrentDirectory,
	FILE_ATTRIBUTE_DIRECTORY, FullPath.GetString(),
	FILE_ATTRIBUTE_DIRECTORY); 
	return CString(ReletivePath);
}

std::wstring FileHelper::GetOperationFilePath(bool FileModeFlag,CWnd*const MFCWndClassPtr)
{
	std::wstring FilePath;

	if (!MFCWndClassPtr)
		return FilePath;

	CFileDialog Dlg(FileModeFlag,// ��� ���� -true��� ������, false��� ������
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





