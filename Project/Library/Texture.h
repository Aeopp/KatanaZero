#pragma once
#include <string>
#include <memory>
#include <Windows.h>

class Texture abstract
{
public:
	explicit Texture() {};
	virtual ~Texture() {};
public:
	virtual HRESULT Insert_Texture(const std::wstring& FilePath,
		const std::wstring& StateKey= L"",
		const uint32_t Count= 0)abstract; 

	// �ؽ��Ŀ� ���� �������� �����մϴ�.
	virtual  std::shared_ptr<class TexInfo> Get_TexInfo(const std::wstring& StateKey = L"",
		const uint32_t Count= 0)abstract;

	virtual void Release()abstract;
};

