#pragma once
#include "Texture.h"
#include <Windows.h>
#include <string>
#include <map>
#include <memory>
#include <vector>

class Multi_Texture final :
	public Texture
{
public:
	explicit Multi_Texture();
	virtual ~Multi_Texture();
public:
	// Zero Index Base [0,Count)
	virtual HRESULT Insert_Texture(const std::wstring & FilePath, const std::wstring & StateKey ,
		const uint32_t Count ) override;
	virtual   std::shared_ptr<class TexInfo> Get_TexInfo(const std::wstring & StateKey ,
	const uint32_t Index ) override;

	virtual void Release() override;
private:
	// Key Equal to StateKey
	std::map<std::wstring, std::vector< std::shared_ptr<class TexInfo>>> _TextureMap;
};

