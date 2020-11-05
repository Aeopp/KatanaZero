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

std::array<vec3, 4ul> math::GetWorldRectPt(matrix MWorld,
	float Width,float Height)
{
	auto LocalPoints = math::GetLocalRect(vec2{ Width,Height });
	std::array<vec3, 4ul> _WorldRectPt;

	std::transform(
		std::move_iterator(std::begin(LocalPoints)),
		std::move_iterator(std::end(LocalPoints) - 1),
		std::begin(_WorldRectPt),
		[&MWorld](auto&& LocalPt) {
		D3DXVec3TransformCoord(&LocalPt, &LocalPt, &MWorld);
		return LocalPt;
	});

	LocalPoints.front();

	// 이후에 바텀과 탑을 뒤집기
	std::swap(_WorldRectPt[0].y, _WorldRectPt[2].y);
	std::swap(_WorldRectPt[1].y, _WorldRectPt[3].y);

	return _WorldRectPt;
}

float math::Angle360conversion(float degree)
{
	if (degree < 0)
		return degree *= -1;
	else
		return (180 + (180 - degree));
}

D3DXCOLOR math::lerp(D3DXCOLOR start, D3DXCOLOR goal, float goal_time, float dt)
{
	D3DXCOLOR _color;

	_color.a = lerp(start.a, goal.a, goal_time, dt);
	_color.r = lerp(start.r, goal.r, goal_time, dt);
	_color.g = lerp(start.g, goal.g, goal_time, dt);
	_color.b = lerp(start.b, goal.b, goal_time, dt);

	return _color;
}



vec3 math::RandVec(std::pair<float, float> Range)
{
	vec3 vec;
	vec.x = math::Rand(Range);
	vec.y = math::Rand(Range);
	vec.z = 0;
	return vec;
}


vec3 math::ConvertXAxisDir(vec3 Dir)
{
	D3DXVec3Normalize(&Dir, &Dir);

	if (Dir.x < 0.f)
		Dir = vec3{ -1.f,0.f,0.f };
	else
		Dir = vec3{ +1.f,0.f,0.f };

	return Dir;
}

float math::AngleFromVec(vec3 Dir)
{
	return std::atan2(Dir.y, Dir.x);
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

	// Screen Coord System Calc
	/*_Rt.top *= -1.f;
	_Rt.bottom *= -1.f;*/

	return _Rt;
}

RECT math::ConvertLocalPtToRECT(const std::pair<vec3, vec3>& LocalPt)
{
	RECT _Rt;

	_Rt.left = LocalPt.first.x;
	_Rt.top = LocalPt.first.y;
	_Rt.right = LocalPt.second.x;
	_Rt.bottom = LocalPt.second.y;

	//// Screen Coord System Calc
	//_Rt.top *= -1.f;
	//_Rt.bottom *= -1.f;
	
	return _Rt;
}
vec3 math::RotationVec(const vec3 Vec,float Degree)
{
	matrix _MRot;
	D3DXMatrixRotationZ(&_MRot, D3DXToRadian(Degree));
	vec3 RotVec;
	D3DXVec3TransformNormal(&RotVec, &Vec, &_MRot);
	return RotVec;
}
vec3 math::GetCenter(const RECT& _Rt)
{
	return vec3
	{
		 ( _Rt.right +_Rt.left  ) /2.f,
		 (_Rt.bottom + _Rt.top) / 2.f, 
		 0.f
	};
}
vec3 math::GetCenter(const std::array<vec3, 4ul> _Points)
{
	return vec3
	{
		 ( _Points[0].x +_Points[2].x  )/2.f ,
		(_Points[0].y + _Points[2].y ) / 2.f  ,
		  0.f 
	};
}

float math::OnlyDecimal(float floating_point)
{
	int32_t Number = floating_point;
	return floating_point - (float)Number;
}

// 화면 좌표가 아님 !! Y 축이 뒤집힘에 유의
std::array<vec3, 5ul> math::GetLocalRect(const vec2 & Size)
{
	const float halfX = Size.x / 2.f;
	const float halfY = Size.y / 2.f;

	std::array<vec3, 5ul> LocalRect;
	LocalRect[0] = vec3{ -halfX,+halfY,0.f };
	LocalRect[1] = vec3{ +halfX,+halfY,0.f };
	LocalRect[2] = vec3{ +halfX,-halfY,0.f };
	LocalRect[3] = vec3{ -halfX,-halfY,0.f };
	LocalRect[4] = vec3{  0.f  , 0.f,0.f };

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
	_RhsRt  = ConvertLocalPtToRECT(RectAndRect.second);

	if (IntersectRect(&_DirRt, &_LhsRt, &_RhsRt))
	{
		vec3 Dir;
		Dir.x = _DirRt.right - _DirRt.left;
		Dir.y = _DirRt.bottom - _DirRt.top;

		if (((_LhsRt.right - _RhsRt.right) < 0))
		{
			Dir.x *= -1.f;
		}
		if ((_LhsRt.bottom - _RhsRt.bottom) < 0)
		{
			Dir.y *= -1.f;
		}

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

typename math::Collision::IsCollision_Dir math::Collision::SegmentAndRect(
	const std::pair<std::pair<vec3, vec3>, std::array<vec3, 4ul>>
	SegmentAndRect, const bool bDirNormal,
	float& PushScala,vec3& PushDir, vec3& Position)
{
	auto& [Segment,Rect ]= SegmentAndRect;

	// 선분의 시작점을  a라 한다 끝점은 b라 한다.
	// a에서 b로 향하는 단위화된 벡터를 r 이라 한다.
	// a와 사각형의 아래 꼭짓점 두 점을 빼서 벡터를 만든다. 이 벡터를 v w 라 한다.
	// r 과 v 를 내적해 선분에 투영된 길이를 구한다. 이 길이를 d 라 한다. 
	// a + (r*d) 를 한다 해당 위치를 y 라 한다.
	// y가 사각형과 내접한다면 사각형과 선분은 충돌상태이다.

	// 상단의 연산을 사각형의 아래 꼭짓점 두개에 동시에 적용해 하나라도 내접한다면 충돌이다.

	/*const vec3 RectCenter{ (Rect[0].x + Rect[2].x) / 2.f,
							(Rect[0].y + Rect[2].y) / 2.f,0.f };*/
	
	vec3 Distance= Segment.second - Segment.first;
	vec3 Line{};
	D3DXVec3Normalize(&Line, &Distance);
	//vec3 ToLB = Rect[3] - Segment.first;
	//vec3 ToRB = Rect[2] - Segment.first;

	float left = min(Segment.first.x, Segment.second.x);
	float top = min(Segment.first.y, Segment.second.y);
	float right = max(Segment.first.x, Segment.second.x);
	float bottom = max(Segment.first.y, Segment.second.y);

	for(int i=0;i<4;++i)
	{
		vec3 ToVectex = Rect[i] - Segment.first;
		float dot = D3DXVec3Dot(&Line, &ToVectex);
		vec3 SegmentPoint = Segment.first + (Line * dot);

		// 직선과 사각형의 충돌
		if (math::IsPointInnerRect(Rect, SegmentPoint))
		{
			if (SegmentPoint.x >= left && SegmentPoint.x <= right
				&& SegmentPoint.y <= bottom && SegmentPoint.y >= top)
			{
				vec3 ToRightDir = Segment.first.x >= Segment.second.x ?
					Segment.first - Segment.second : Segment.second - Segment.first;
				if (bDirNormal)
					 D3DXVec3Normalize(&ToRightDir, &ToRightDir);

				// 사각형의 좌우 꼭짓점의 중간에 대응하는 선분의 중앙 점
				vec3 SegmentCenter =SegmentPoint + Segment.first + (Line * dot);
				SegmentCenter *= 0.5f;
				Position = SegmentCenter;
				const vec3 RectCenter = math::GetCenter(Rect);
				PushDir =RectCenter - SegmentCenter;
				vec3 RectDiagonal{ 0.f,0.f,0.f };
				RectDiagonal.x = Rect[1].x - Rect[3].x;
				RectDiagonal.y = Rect[1].y - Rect[3].y;
				float Diagonal  =D3DXVec3Length(&RectDiagonal);
				Diagonal - D3DXVec3Length(&PushDir);
				D3DXVec3Normalize(&PushDir, &PushDir);

				return ToRightDir;
			}
		}
	}



	return std::nullopt;
}
