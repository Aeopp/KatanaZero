#include "stdafx.h"
#include "global.h"
std::array<vec3, 4ul> global::GetScreenRect()
{
	return{ vec3{ 0.f,0.f,0.f },
			vec3{ ClientSize.first,0.f,0.f },
			vec3{ ClientSize.first,ClientSize.second,0.f } ,
			vec3{ 0.f,ClientSize.second,0.f } , };
}


