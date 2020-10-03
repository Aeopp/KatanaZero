#pragma once
#include <utility>
#include <random>
#include <array>
#include "Typedef.h"

class math
{
private:
	static const std::random_device rd;
public:
	static constexpr float PI = 3.14159265f;

	template<typename T>
	auto LOOP(std::pair<T, T> Range, T Target);

	template<typename Ty>
	static Ty Rand_Normal(std::pair<Ty, Ty> range);

	template<typename Ty>
	static Ty Rand(std::pair<Ty, Ty> range);

	static float lerp(float start, float goal, float goal_time, float dt);
	static float Angle360conversion(float degree);

	// 사각형의 회전여부는 상관 없습니다.
	static bool IsPointInnerRect(const std::array<vec3, 4>& RectPoint, const vec3& Point);
};




template<typename T>
auto math::LOOP(std::pair<T, T> Range, T Target)
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
