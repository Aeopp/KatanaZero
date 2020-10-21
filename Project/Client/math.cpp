#include "stdafx.h"
#include "math.h"


matrix math::GetCameraJoomMatrix(const float JoomScale, const vec3 & ScreenSize)
{
	matrix MCameraRelativeTranslation, MJoomScale, MScreenReturn;
	const float halfX = ScreenSize.x / 2.f;
	const float halfY = ScreenSize.y / 2.f;
	D3DXMatrixTranslation(&MCameraRelativeTranslation, -halfX, -halfY, 0.f);
	D3DXMatrixScaling(&MJoomScale, JoomScale, JoomScale, 0.f);
	D3DXMatrixTranslation(&MScreenReturn, +halfX, +halfY, 0.f);
	return MCameraRelativeTranslation*MJoomScale*MScreenReturn;
}

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

vec3 math::lerp(const vec3 start, const vec3 goal, float goal_time, float dt)
{
	vec3 returnVec;
	returnVec.x = lerp(start.x, goal.x, goal_time, dt);
	returnVec.y = lerp(start.y, goal.y, goal_time, dt);
	returnVec.z = 0.f;
	return returnVec;
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

RECT math::ConvertLocalPtToRECT(const std::array<vec3, 4ul>& LocalPt)
{
	RECT _Rt;

	_Rt.left = LocalPt[0].x;
	_Rt.top = LocalPt[0].y;
	_Rt.right = LocalPt[2].x;
	_Rt.bottom = LocalPt[2].y;

	return _Rt;
}

RECT math::ConvertLocalPtToRECT(const std::pair<vec3, vec3>& LocalPt)
{
	RECT _Rt;

	_Rt.left = LocalPt.first.x;
	_Rt.top = LocalPt.first.y;
	_Rt.right = LocalPt.second.x;
	_Rt.bottom = LocalPt.second.y;

	return _Rt;
}

std::array<vec3, 5ul> math::GetLocalRect(const vec2 & Size)
{
	const float halfX = Size.x / 2.f;
	const float halfY = Size.y / 2.f;

	std::array<vec3, 5ul> LocalRect;
	LocalRect[0] = vec3{ -halfX,+halfY,0.f };
	LocalRect[1] = vec3{ +halfX,+halfY,0.f };
	LocalRect[2] = vec3{ +halfX,-halfY,0.f };
	LocalRect[3] = vec3{ -halfX,-halfY,0.f };
	LocalRect[4] = vec3{ halfX  , halfY,0.f };

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
(const std::pair<std::array<vec3, 4>, std::pair<vec3, float>>& RectAndCircle, const bool bDirNormal)
{
	for (const auto& RectPoint : RectAndCircle.first)
	{
		const float CurrentDistance = math::GetPointDistance({ RectPoint,RectAndCircle.second.first});
		if (CurrentDistance < RectAndCircle.second.second)
		{
			const float x = RectAndCircle.first[0].x + ((RectAndCircle.first[2].x - RectAndCircle.first[0].x) / 2.f);
			const float y = RectAndCircle.first[0].y + ((RectAndCircle.first[2].y - RectAndCircle.first[0].y) / 2.f);

			vec3 RectCenter = { x,y,0.f };
			vec3 DistanceVec = RectCenter - RectAndCircle.second.first;
			
			if(bDirNormal)
				D3DXVec3Normalize(&DistanceVec, &DistanceVec);

			return DistanceVec;
		}
	}
	return std::nullopt;
}

typename math::Collision::IsCollision_Dir 
math::Collision::RectAndRect(
	const std::pair<std::array<vec3, 4>, 
	std::array<vec3, 4>>& RectAndRect,const bool bDirNormal)
{
	typename math::Collision::IsCollision_Dir opOutDir;
	
	RECT _LhsRt,_RhsRt,_DirRt;
	_LhsRt  = ConvertLocalPtToRECT(RectAndRect.first); 
	_RhsRt   = ConvertLocalPtToRECT(RectAndRect.second);
 
	if (IntersectRect(&_DirRt, &_LhsRt, &_RhsRt))
	{
		vec3 Dir;
		Dir.x = _DirRt.right - _DirRt.left;
		Dir.y = _DirRt.bottom - _DirRt.top;

		if(bDirNormal)
			D3DXVec3Normalize(&Dir, &Dir);

		return Dir;
	}

	return  std::nullopt;
}

typename math::Collision::IsCollision_Dir 
math::Collision::CircleAndCircle
(const std::pair<std::pair<vec3, float>, std::pair<vec3, float>> CircleAndCircle, const bool bDirNormal)
{
	vec3 DistanceVec = CircleAndCircle.first.first - CircleAndCircle.second.first;
	const float Distance =  D3DXVec3Length(&DistanceVec);
	bool IsCollision = Distance < (CircleAndCircle.first.second + CircleAndCircle.second.second);
	vec3 OutNormalDir{0.f,0.f,0.f};

	if (IsCollision)
	{
		if(bDirNormal)
			D3DXVec3Normalize(&DistanceVec, &DistanceVec);
		
		return DistanceVec;
	}
	
	return std::nullopt;
}
