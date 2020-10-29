#pragma once
#include <utility>
#include <random>
#include <array>
#include <forward_list>
using float_t = float;

class math
{
private:
	static const std::random_device rd;
public:
	static constexpr float PI = 3.14159265f;

	template<typename Type>
	auto LOOP(std::pair<Type, Type> Range, Type Target);

	template<typename Ty>
	static Ty Rand_Normal(std::pair<Ty, Ty> range);

	template<typename Ty>
	static Ty Rand(std::pair<Ty, Ty> range);

	static float lerp(float start, float goal, float goal_time, float dt);

	static float Angle360conversion(float degree);

	// 사각형의 회전여부는 상관 없습니다.
	static bool IsPointInnerRect(const std::array<vec3, 4>& RectPoint, const vec3& Point);
	static float_t GetPointDistance(const std::pair<vec3,vec3>& TargetPoints);

	// 데카르트 좌표계
	static std::array<vec3, 4ul> GetLocalRect(const vec2 & Size/*전체사이즈 입력 (절반이 아님)*/);

	static matrix GetCameraJoomMatrix(const float JoomScale,
		/*전체 사이즈 입력(절반아님)*/const vec3& ScreenSize);
	class Collision
	{
		// 사각형과 원충돌
		// 원과 원
	public : 
		// 충돌 여부와 중점간의 방향 (Lhs<-Rhs)
		using IsCollision_Dir = std::pair<bool, vec3>;
		// 회전한 사각형 끼리 충돌
		/*static IsCollision_Dir RectAndRect(
		const std::pair<std::array<vec3,4>,std::array<vec3,4>>& Rects);*/
		
		static IsCollision_Dir RectAndCircle(
			      //           렉트                 // 서클 (중점 반지름)
		const std::pair<std::array<vec3, 4>, std::pair<vec3, float>>& RectAndCircle);

		static IsCollision_Dir CircleAndCircle(
			const std::pair<std::pair<vec3, float>, std::pair<vec3, float>> CircleAndCircle);
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
