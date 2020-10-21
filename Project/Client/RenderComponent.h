#pragma once
#include "Component.h"
#include <optional>

class RenderComponent : public Component
{
public :
	void Render()override;
	// Y 기준은 오브젝트의 최하단
	std::optional<float>CalcY();
	//
	struct RenderInfo
	{
		std::wstring ObjectKey;
		std::wstring StateKey;
		uint8_t Number;
		vec2 SrcScale{ 1.f,1.f };
		D3DCOLOR _Color = D3DCOLOR_ARGB(255, 255, 255, 255);
		LAYER::ELAYER _Layer;
	};

	RenderInfo _RenderInfo{};
private:
	bool bRenderControl = false; 
};

