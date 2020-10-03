#pragma once
#include "Texture.h"
#include <string>

class Single_Texture final:
	public Texture
{
public:
	explicit Single_Texture();
	virtual ~Single_Texture();

	virtual HRESULT Insert_Texture(const std::wstring & FilePath, 
	const std::wstring & StateKey = L"", 
	const uint32_t Count = 0) override;

	virtual std::shared_ptr<class TexInfo> Get_TexInfo(const std::wstring & Statekey = L"",
		const uint32_t Count = 0) override;
	virtual void Release() override;
private:
	std::shared_ptr<class TexInfo> _TexInfo{};
};

