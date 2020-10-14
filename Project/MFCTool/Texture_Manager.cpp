#include "stdafx.h"
#include "Texture_Manager.h"
#include "Single_Texture.h"
#include "Multi_Texture.h"
#include <fstream>
#include "TupleGlobalHelper.h"

Texture_Manager::~Texture_Manager()noexcept
{

}

std::shared_ptr<class TexInfo> Texture_Manager::Get_TexInfo(const wstring & ObjectKey,
const wstring & StateKey /*=L""*/, const DWORD & Index /*= 0*/)
{
	auto iter_find = TextureMap.find(ObjectKey);
	if (TextureMap.end() == iter_find)
		return nullptr; 

	return iter_find->second->Get_TexInfo(StateKey, Index);
}

HRESULT Texture_Manager::InsertTexture(const TEX_ID ETexID, 
const wstring & FilePath, const wstring & ObjectKey, 
const wstring & StateKey/*=L""*/, const DWORD & Index/*=0*/)
{
	auto iter_find = TextureMap.find(ObjectKey); 

	if (TextureMap.end() == iter_find)
	{
		Texture* pTexture = nullptr;
		
		if (ETexID == SINGLE_TEX)
		{
			pTexture = new Single_Texture;

			if (FAILED(pTexture->Insert_Texture(FilePath, StateKey, Index)))
			{
				delete pTexture;
				ERR_MSG(L"SINGLE - Texture Insert Failed - Texture_Manager.cpp");
				return E_FAIL;
			}

			std::shared_ptr<Single_Texture> sp_Texture;
			sp_Texture.reset(dynamic_cast<Single_Texture*>(pTexture ) );
			TextureMap.emplace(ObjectKey, std::move(sp_Texture));

		}
		else if (ETexID == MULTI_TEX)
		{
			pTexture = new Multi_Texture;

			if (FAILED(pTexture->Insert_Texture(FilePath, StateKey, Index)))
			{
				delete pTexture;
				ERR_MSG(L"Texture Insert Failed - Texture_Manager.cpp");
				return E_FAIL;
			}

			std::shared_ptr<Multi_Texture> sp_Texture;
			sp_Texture.reset(dynamic_cast<Multi_Texture*>(pTexture));

			TextureMap.emplace(ObjectKey, std::move(sp_Texture));
		}

	}
	else if (MULTI_TEX == ETexID)
	{
		if (FAILED(TextureMap[ObjectKey]->Insert_Texture(FilePath, StateKey, Index)))
		{
			ERR_MSG(L"MultiTexture Insert Failed - Texture_Manager.cpp");

			return E_FAIL; 
		}
	}
	return S_OK;
}

void Texture_Manager::LoadTexturesFromTexInfoFile(const std::wstring_view FileNameView) & noexcept
{
	using namespace TupleGlobalHelper;

	std::wifstream FileIn;
	FileIn.open(FileNameView.data());

	if (!FileIn.fail())
	{
		while (true)
		{
			if (FileIn.eof())
				break;
			TexPath _TexPath;
			FileIn >> _TexPath.RefTuple();

			if (!_TexPath.Count) continue;

			if (FAILED(InsertTexture(Texture_Manager::MULTI_TEX, _TexPath.RelativePath,
				_TexPath.ObjectKey, _TexPath.StateKey, _TexPath.Count)))
			{
				ERR_MSG(__FUNCTIONW__);
			}
		}
	}
}

void Texture_Manager::Release()
{
	TextureMap.clear(); 
}
