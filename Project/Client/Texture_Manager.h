#pragma once
#include "singleton_interface.h"
#include <string_view>
class TextureManager : public singleton_interface<TextureManager>
{
public:
	virtual ~TextureManager()noexcept;
public:
	enum TEX_ID {SINGLE_TEX, MULTI_TEX};
public:
	// �̱� �ؽ����� ��� 2��° �Ķ���ͺ��� �ǹ̰� �����ϴ�.
	std::shared_ptr<class TexInfo> Get_TexInfo
(const std::wstring& ObjectKey, const std::wstring& StateKey = L"", /*Zero Base [0,Index)*/const DWORD& Index = 0);

	
	HRESULT InsertTexture(const TEX_ID ETexID, const std::wstring& FilePath, const std::wstring& ObjectKey,
	const std::wstring& StateKey = L"", const DWORD& Index = 0);

	void LoadTexturesFromTexInfoFile(const std::wstring& FileName)&noexcept;

	void Release(); 
	// ���⼭�� wstring�� ������ƮŰ - ��ü - �÷��̾�, npc, ���� 1, ����2 ����3, ��� 23~
	std::map<std::wstring,std::shared_ptr<class Texture>> TextureMap; 
};

