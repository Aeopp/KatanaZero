#pragma once
#include "Struct.h"
#include <vector> 
class FileHelper
{
public:
	// 지정한 경로를 상대경로로 변환하며 드라이브가 다를시 동작은 알 수 없습니다.
	// 문자열이 비어있는 경우를 체크해주세요.
	// TRUE = READ  FALSE = SAVE
	static std::wstring GetOperationFilePath(bool FileModeFlag, CWnd* const MFCWndClassPtr);
	static CString ConvertReletivePath(CString FullPath);
	static std::vector<TexPath> DirectoryInfoExtraction(const std::wstring& FullPath)noexcept;
	static uint32_t CalcDirectoryFileCount(const std::wstring& FullPath);
};

