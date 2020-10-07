#pragma once

class FileHelper
{
public:
	// ������ ��θ� ����η� ��ȯ�ϸ� ����̺갡 �ٸ��� ������ �� �� �����ϴ�.
	static CString ConvertReletivePath(CString FullPath);
	// ���ڿ��� ����ִ� ��츦 üũ���ּ���.
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

