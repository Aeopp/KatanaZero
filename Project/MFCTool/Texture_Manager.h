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
	// 싱글 텍스쳐의 경우 2번째 파라미터부턴 의미가 없습니다.
	std::shared_ptr<class TexInfo> Get_TexInfo
(const wstring& ObjectKey, const wstring& StateKey = L"", /*Zero Base [0,Index)*/const DWORD& Index = 0);

	
	HRESULT InsertTexture(const TEX_ID ETexID, const wstring& FilePath, const wstring& ObjectKey,
	const wstring& StateKey = L"", const DWORD& Index = 0);

	void LoadTexturesFromTexInfoFile(const std::wstring_view FileNameView)&noexcept;

	void Release(); 
	// 여기서의 wstring은 오브젝트키 - 객체 - 플레이어, npc, 보스 1, 보스2 보스3, 잡몹 23~
	map<wstring,std::shared_ptr<class Texture>> TextureMap; 
};

