#pragma once
#include "Texture.h"

class Multi_Texture final :
	public Texture
{
public:
	explicit Multi_Texture();
	virtual ~Multi_Texture();
public:
	// Zero Index Base [0,Count)
	virtual HRESULT Insert_Texture(const wstring & FilePath, const wstring & StateKey ,
		const uint32_t Count ) override;
	virtual   std::shared_ptr<class TexInfo> Get_TexInfo(const wstring & StateKey ,
	const uint32_t Index ) override;

	virtual void Release() override;
private:
	// Key Equal to StateKey
	map<wstring, vector< std::shared_ptr<class TexInfo>>> _TextureMap;
};

