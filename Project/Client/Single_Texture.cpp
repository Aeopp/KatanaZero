#include "stdafx.h"
#include "Single_Texture.h"
#include "GraphicDevice.h"


Single_Texture::Single_Texture()
{
}


Single_Texture::~Single_Texture()
{
	Release();
}

HRESULT Single_Texture::Insert_Texture(
const std::wstring & FilePath, const std::wstring & Statekey /*= L""*/, const uint32_t  Count /*= 0*/)
{
	_TexInfo = std::make_shared<TexInfo>();
	if(!_TexInfo)return E_FAIL;

	if (FAILED(D3DXGetImageInfoFromFile(FilePath.c_str(), 
	&_TexInfo->ImageInfo)))
		return E_FAIL; 

	if (FAILED(D3DXCreateTextureFromFileEx
	   (GraphicDevice::instance().GetDevice(),
		FilePath.c_str(),
		_TexInfo->ImageInfo.Width,
		_TexInfo->ImageInfo.Height,
		_TexInfo->ImageInfo.MipLevels,
		0,
		_TexInfo->ImageInfo.Format,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		nullptr,
		nullptr,
		&_TexInfo->pTexture)))
	{
		ERR_MSG(L"Create Texture Failed! - SingleTexture.cpp");
		return E_FAIL;
	}
	return S_OK;
}

std::shared_ptr<class TexInfo> Single_Texture::Get_TexInfo
(const std::wstring& StateKey /*= L""*/, const uint32_t Count /*= 0*/)
{
	return _TexInfo;
}

void Single_Texture::Release()
{

}
