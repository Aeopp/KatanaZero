#pragma once

class FileHelper
{
public:
	// 지정한 경로를 상대경로로 변환하며 드라이브가 다를시 동작은 알 수 없습니다.
	static CString ConvertReletivePath(CString FullPath);
	// 문자열이 비어있는 경우를 체크해주세요.
	// TRUE = READ  FALSE = SAVE
	static std::wstring GetOperationFilePath(bool FileModeFlag,CWnd*const MFCWndClassPtr);
};

class CFileInfo
{
public:
	CFileInfo();
	~CFileInfo();

public:
	static CString ConvertReletivePath(CString strFullPath);
};

