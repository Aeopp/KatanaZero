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

float_t math::GetPointDistance(const std::pair<vec3, vec3>& TargetPoints)
{
	return D3DXVec3Length(&(TargetPoints.first - TargetPoints.second));
}

std::array<vec3, 4ul> math::GetLocalRect(const vec2 & Size)
{
	const float halfX = Size.x / 2.f;
	const float halfY = Size.y / 2.f;

	std::array<vec3, 4ul> LocalRect;
	LocalRect[0] = vec3{ -halfX,+halfY,0.f };
	LocalRect[1] = vec3{ +halfX,+halfY,0.f };
	LocalRect[2] = vec3{ +halfX,-halfY,0.f };
	LocalRect[3] = vec3{ -halfX,-halfY,0.f };

	return LocalRect;
}

//typename math::Collision::IsCollision_Dir 
//math::Collision::RectAndRect
//(const std::pair<std::array<vec3, 4>, std::array<vec3, 4>>& Rects)
//
//{
//	vec3 OutDir{ 0.f,0.f,0.f };
//
//	for (const auto& RectPoint : Rects.first)
//	{
//		
//	}
//}

typename math::Collision::IsCollision_Dir 
math::Collision::RectAndCircle
(const std::pair<std::array<vec3, 4>, std::pair<vec3, float>>& RectAndCircle)

{
	vec3 OutDir{ 0.f,0.f,0.f };

	for (const auto& RectPoint : RectAndCircle.first)
	{
		const float CurrentDistance = math::GetPointDistance({ RectPoint,RectAndCircle.second.first});
		if (CurrentDistance < RectAndCircle.second.second)
		{
			const float x = RectAndCircle.first[0].x + ((RectAndCircle.first[2].x - RectAndCircle.first[0].x) / 2.f);
			const float y = RectAndCircle.first[0].y + ((RectAndCircle.first[2].y - RectAndCircle.first[0].y) / 2.f);

			vec3 RectCenter = { x,y,0.f };
			vec3 DistanceVec = RectCenter - RectAndCircle.second.first;
			D3DXVec3Normalize(&OutDir, &DistanceVec);

			return{ true ,OutDir };
		}
	}
	return{ false ,OutDir };
}

typename math::Collision::IsCollision_Dir 
math::Collision::CircleAndCircle
(const std::pair<std::pair<vec3, float>, std::pair<vec3, float>> CircleAndCircle)

{
	vec3 DistanceVec = CircleAndCircle.first.first - CircleAndCircle.second.first;
	const float Distance =  D3DXVec3Length(&DistanceVec);
	bool IsCollision = Distance < (CircleAndCircle.first.second + CircleAndCircle.second.second);
	vec3 OutNormalDir{0.f,0.f,0.f};

	if (IsCollision)
		D3DXVec3Normalize(&OutNormalDir, &DistanceVec);
	
	return IsCollision_Dir{ IsCollision,OutNormalDir };
}
