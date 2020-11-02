
#include "Define.h"
#include "stdafx.h"

HWND global::hWND = nullptr;
bool global::bDebug = true;
std::pair<float, float> global::WorldMousePos = std::pair<float, float>{ 0,0 };

std::array<vec3, 4ul> global::GetScreenRect()
{
	return{ vec3{ 0.f,0.f,0.f },
			vec3{ ClientSize.first,0.f,0.f },
			vec3{ ClientSize.first,ClientSize.second,0.f } ,
			vec3{ 0.f,ClientSize.second,0.f } , };
}

