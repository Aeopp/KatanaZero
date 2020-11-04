#pragma once
#include <utility>
#include <random>
#include <array>
#include <forward_list>
#include <optional>
#include <memory>
#include "Tags.h"
#include <any>

using float_t = float;




class math
{
private:
	static  inline std::random_device rd;
public:
	static constexpr float GRAVITY = 300.f;
	static constexpr float GRAVITY_ACC = 100.f;
	static constexpr float PI = 3.14159265f;

	template<typename Type>
	auto LOOP(std::pair<Type, Type> Range, Type Target);

	static vec3 RandVec( std::pair<float, float> Range);

	template<typename Ty>
	static Ty Rand_Normal(std::pair<Ty, Ty> range);

	static vec3 ConvertXAxisDir(vec3 Dir);;
	template<typename Ty>
	static Ty Rand(std::pair<Ty, Ty> range);

	static float AngleFromVec(vec3 Dir);
	static float lerp(float start, float goal, float goal_time, float dt);
	static vec3 lerp(const vec3 start, const vec3 goal, float goal_time, float dt);
	static D3DXCOLOR lerp(D3DXCOLOR start, D3DXCOLOR goal, float goal_time, float dt);

	template<class T>
	typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
		static inline almost_equal(T x, T y, int ulp)
	{
		return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp|| std::fabs(x - y) < (std::numeric_limits<T>::min)();
	}

	static bool almost_equal(const vec3& lhs, const vec3& rhs)
	{
		return
			math::almost_equal(lhs.x, rhs.x, 1l) &&
			math::almost_equal(lhs.y, rhs.y, 1l) &&
			math::almost_equal(lhs.z, rhs.z, 1l);
	}

	static float Angle360conversion(float degree);

	// 사각형의 회전여부는 상관 없습니다.
	static bool IsPointInnerRect(const std::array<vec3, 4>& RectPoint, const vec3& Point);
	static float_t GetPointDistance(const std::pair<vec3,vec3>& TargetPoints);

	static RECT ConvertLocalPtToRECT(const std::array<vec3, 4ul>& LocalPt);
	static RECT ConvertLocalPtToRECT(const std::pair<vec3, vec3>& LocalPt);

	// 단위는 디그리 회전 방향은 양의 x축에서 봤을때의 시계방향
	static vec3 RotationVec(const vec3 Vec,float Degree);

	static vec3 GetCenter(const RECT& _Rt);
	static vec3 GetCenter(const std::array<vec3, 4ul > _Points);

	static float OnlyDecimal(float floating_point);
	// 데카르트 좌표계
	// 마지막 포인트는 로컬좌표의 정중앙을 의미
	static std::array<vec3, 5ul> GetLocalRect(const vec2 & Size/*전체사이즈 입력 (절반이 아님)*/);

	static matrix GetCameraJoomMatrix(const float JoomScale,
		/*전체 사이즈 입력(절반아님)*/const vec3& ScreenSize);
	static std::array<vec3, 4ul> GetWorldRectPt(matrix MWorld,float Width, float Height);
	class Collision
	{
		// 사각형과 원충돌
		// 원과 원
	public : 
		// 충돌 여부와 중점간의 방향 (Lhs<-Rhs)
		using IsCollision_Dir = std::optional<vec3>;
		// 회전한 사각형 끼리 충돌
		/*static IsCollision_Dir RectAndRect(
		const std::pair<std::array<vec3,4>,std::array<vec3,4>>& Rects);*/
		
		// 렉트 원소 순서 : LT , RT , RB , LB 

		static IsCollision_Dir RectAndCircle(
		const std::pair<std::array<vec3, 4>, std::pair<vec3, float>>& RectAndCircle,
			const bool bDirNormal);

		static IsCollision_Dir RectAndRect(
			const std::pair<std::array<vec3, 4>, std::array<vec3, 4>>& RectAndRect,
			const bool bDirNormal);

		static IsCollision_Dir CircleAndCircle(
			const std::pair<std::pair<vec3, float>, std::pair<vec3, float>> CircleAndCircle,
			const bool bDirNormal);

		// 반환값은 기울기를 나타냅니다.
		// 반환값이 바라보는 방향은 항상 ->  왼쪽에서 오른쪽으로 약속
		static IsCollision_Dir SegmentAndRect(
			const std::pair<    std::pair<vec3, vec3>, std::array<vec3, 4ul>  > SegmentAndRect,
			const bool bDirNormal,
			// 밀어내야 하는 양
			float& PushScala,
			// 밀어내야 하는 방향
			vec3& PushDir,
			// 사각형의 아래부분 꼭짓점 두부분의 중간에 대응하는 선분의 교점
			vec3& Position);
		
		struct HitInfo
		{
			OBJECT_ID::EID _ID{};
			OBJECT_TAG::ETAG _TAG{};
			// 노말
			vec3 Normal{};
			// 부딪힌 물체가 나를 바라보는 방향
			vec3 PushDir{ 1.f,0.f,0.f };
			vec3 PosDir{};
			vec3 CrossingAreaDir{};
			float PosDistance{};
			// 부딪힌 물체의 중심점
			vec3 Position{};
			float PushForce= 100.f;
			// 부딪힌 물체와 교차하는 영역 만큼의 거리
			float IntersectAreaScale = 0.f;
			bool IsLhs = false;
			// 타겟
			std::weak_ptr<class object> _Target{};
			std::any _Variable{};
		};
	};
};




template<typename Type>
auto math::LOOP(std::pair<Type, Type> Range, Type Target)
{
	auto [left, right] = Range;

	
	if (Target <= left)
	{
		return right - (left - Target);
	}
	else if (Target >= right)
	{
		return (Target - right) + left;
	}
}

template<typename Ty>
Ty math::Rand_Normal(std::pair<Ty, Ty> range)
{
	static std::mt19937 gen(rd());

	if constexpr (std::is_floating_point_v<Ty>)
	{
		std::uniform_real_distribution<Ty> dis(range.first, range.second);
		return dis(gen);
	}
	else {
		std::uniform_int_distribution<Ty> dis(range.first, range.second);
		return dis(gen);
	}
}

template<typename Ty>
Ty math::Rand(std::pair<Ty, Ty> range)
{
	static std::mt19937 gen(rd());

	if constexpr (std::is_floating_point_v<Ty>)
	{
		std::uniform_real<Ty> dis(range.first, range.second);
		return dis(gen);
	}
	else {
		std::uniform_int<Ty> dis(range.first, range.second);
		return dis(gen);
	}
}
