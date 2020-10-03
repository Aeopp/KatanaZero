
#include "Multi_Texture.h"
#include "Graphic_Device.h"
#include "global.h"
#include "Struct.h"


Multi_Texture::Multi_Texture()
{
}


Multi_Texture::~Multi_Texture()
{
	Release();
}

HRESULT Multi_Texture::Insert_Texture(const std::wstring & FilePath, 
const std::wstring & StateKey, const uint32_t Count)
{
	auto iter_find = _TextureMap.find(StateKey);

	if (iter_find != _TextureMap.end())
		return E_FAIL;

	TCHAR FileFullPath[MAX_PATH] = L"";

	for (DWORD i = 0; i < Count; ++i)
	{
		swprintf_s(FileFullPath, FilePath.c_str(), i);
		std::shared_ptr<TexInfo> sp_TexInfo = std::make_shared<TexInfo>();

		if (FAILED(D3DXGetImageInfoFromFile(FileFullPath, &sp_TexInfo->ImageInfo)))
			return E_FAIL;
		if (FAILED(D3DXCreateTextureFromFileEx(GraphicDevice::instance().GetDevice(),
			FileFullPath,
			sp_TexInfo->ImageInfo.Width,
			sp_TexInfo->ImageInfo.Height,
			sp_TexInfo->ImageInfo.MipLevels,
			0,
			sp_TexInfo->ImageInfo.Format,
			D3DPOOL_MANAGED,
			D3DX_DEFAULT, D3DX_DEFAULT, 0,
			nullptr, nullptr,
			&sp_TexInfo->pTexture)))
		{
			std::wstring ErrorMsg = FileFullPath;
			ErrorMsg += L"Create Failed";
			ERR_MSG(ErrorMsg.c_str());
			return E_FAIL;
		}
		_TextureMap[StateKey].push_back(std::move(sp_TexInfo));
	}
	return S_OK;
}

std::shared_ptr<class TexInfo> Multi_Texture::Get_TexInfo(const std::wstring & StateKey, 
	const uint32_t Index)
{
	auto& iter_find = _TextureMap.find(StateKey);

	if (_TextureMap.end() == iter_find)
		return nullptr;
	if (iter_find->second.size() <= Index)
		return nullptr;

	return iter_find->second[Index];
}


void Multi_Texture::Release()
{
	for (auto& StateKey_TextureMap : _TextureMap)
	{
		StateKey_TextureMap.second.clear();
		StateKey_TextureMap.second.shrink_to_fit();
	}
	_TextureMap.clear();
}
