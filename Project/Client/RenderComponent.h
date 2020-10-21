#pragma once
#include "Component.h"


class RenderComponent : public Component
{
public :
	void Render()override;

	//
	struct RenderInfo
	{
		std::wstring ObjectKey;
		std::wstring StateKey;
		uint8_t Number;

		LAYER::ELAYER _Layer;
		//vec3 Scale{ 1.f,1.f,0.f };
	};

	RenderInfo _RenderInfo;
};

