#pragma once
#include "singleton_interface.h"

class Texture_Manager : public singleton_interface<Texture_Manager>
{
public:
	virtual ~Texture_Manager()noexcept;
public:
	enum TEX_ID {SINGLE_TEX, MULTI_TEX};
public:
	// �̱� �ؽ����� ��� 2��° �Ķ���ͺ��� �ǹ̰� �����ϴ�.
	std::shared_ptr<class TexInfo> Get_TexInfo
(const wstring& ObjectKey, const wstring& StateKey = L"", /*Zero Base [0,Index)*/const DWORD& Index = 0);

	
	HRESULT InsertTexture(const TEX_ID ETexID, const wstring& FilePath, const wstring& ObjectKey,
	const wstring& StateKey = L"", const DWORD& Index = 0);

	void Release(); 
	// ���⼭�� wstring�� ������ƮŰ - ��ü - �÷��̾�, npc, ���� 1, ����2 ����3, ��� 23~
	map<wstring,std::shared_ptr<class Texture>> TextureMap; 
};

