#pragma once
#include "Struct.h"
#include <vector> 
class FileHelper
{
public:
	// ������ ��θ� ����η� ��ȯ�ϸ� ����̺갡 �ٸ��� ������ �� �� �����ϴ�.
	// ���ڿ��� ����ִ� ��츦 üũ���ּ���.
	// TRUE = READ  FALSE = SAVE
	static std::wstring GetOperationFilePath(bool FileModeFlag, CWnd* const MFCWndClassPtr);
	static CString ConvertReletivePath(CString FullPath);
	static std::vector<TexPath> DirectoryInfoExtraction(const std::wstring& FullPath)noexcept;
	static uint32_t CalcDirectoryFileCount(const std::wstring& FullPath);
};

