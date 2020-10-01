#include "stdafx.h"
#include "math.h"



float math::Angle360conversion(float degree)
{
	if (degree < 0)
		return degree *= -1;
	else
		return (180 + (180 - degree));
}



float math::lerp(float start, float goal, float goal_time, float dt)
{
	float f = goal - start;
	return start + ((f * dt) / goal_time);
}

bool math::IsPointInnerRect(const std::array<vec3, 4>& RectPoint, const vec3& Point)
{
	for (int32_t i = 0; i < 4; ++i)
	{
		vec3 vRhombusDir, vRhombusNormal, vMouseDir;

		vRhombusDir = RectPoint[(i + 1) % 4] - RectPoint[i];
		vRhombusNormal = { -vRhombusDir.y, vRhombusDir.x, 0.f };
		vMouseDir = Point - RectPoint[i];

		
		if (0 > D3DXVec3Dot(&vRhombusNormal, &vMouseDir))
			return false;
	}

	return true;
}


