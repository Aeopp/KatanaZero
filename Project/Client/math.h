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
	static const std::random_device rd;
public:
	static constexpr float GRAVITY = 800.f;
	static constexpr float PI = 3.14159265f;

	template<typename Type>
	auto LOOP(std::pair<Type, Type> Range, Type Target);

	template<typename Ty>
	static Ty Rand_Normal(std::pair<Ty, Ty> range);

	template<typename Ty>
	static Ty Rand(std::pair<Ty, Ty> range);

	static float lerp(float start, float goal, float goal_time, float dt);
	static vec3 lerp(const vec3 start, const vec3 goal, float goal_time, float dt);
	static float Angle360conversion(float degree);

	// �簢���� ȸ�����δ� ��� �����ϴ�.
	static bool IsPointInnerRect(const std::array<vec3, 4>& RectPoint, const vec3& Point);
	static float_t GetPointDistance(const std::pair<vec3,vec3>& TargetPoints);

	static RECT ConvertLocalPtToRECT(const std::array<vec3, 4ul>& LocalPt);
	static RECT ConvertLocalPtToRECT(const std::pair<vec3, vec3>& LocalPt);

	// ������ ��׸� ȸ�� ������ ���� x�࿡�� �������� �ð����
	static vec3 RotationVec(const vec3 Vec,float Degree);

	static vec3 GetCenter(const RECT& _Rt);
	static vec3 GetCenter(const std::array<vec3, 4ul > _Points);

	// ��ī��Ʈ ��ǥ��
	// ������ ����Ʈ�� ������ǥ�� ���߾��� �ǹ�
	static std::array<vec3, 5ul> GetLocalRect(const vec2 & Size/*��ü������ �Է� (������ �ƴ�)*/);

	static matrix GetCameraJoomMatrix(const float JoomScale,
		/*��ü ������ �Է�(���ݾƴ�)*/const vec3& ScreenSize);
	class Collision
	{
		// �簢���� ���浹
		// ���� ��
	public : 
		// �浹 ���ο� �������� ���� (Lhs<-Rhs)
		using IsCollision_Dir = std::optional<vec3>;
		// ȸ���� �簢�� ���� �浹
		/*static IsCollision_Dir RectAndRect(
		const std::pair<std::array<vec3,4>,std::array<vec3,4>>& Rects);*/
		
		// ��Ʈ ���� ���� : LT , RT , RB , LB 

		static IsCollision_Dir RectAndCircle(
		const std::pair<std::array<vec3, 4>, std::pair<vec3, float>>& RectAndCircle,
			const bool bDirNormal);

		static IsCollision_Dir RectAndRect(
			const std::pair<std::array<vec3, 4>, std::array<vec3, 4>>& RectAndRect,
			const bool bDirNormal);

		static IsCollision_Dir CircleAndCircle(
			const std::pair<std::pair<vec3, float>, std::pair<vec3, float>> CircleAndCircle,
			const bool bDirNormal);

		// ��ȯ���� ���⸦ ��Ÿ���ϴ�.
		// ��ȯ���� �ٶ󺸴� ������ �׻� ->  ���ʿ��� ���������� ���
		static IsCollision_Dir SegmentAndRect(
			const std::pair<    std::pair<vec3, vec3>, std::array<vec3, 4ul>  > SegmentAndRect,
			const bool bDirNormal,
			// �о�� �ϴ� ��
			float& PushScala,
			// �о�� �ϴ� ����
			vec3& PushDir,
			// �簢���� �Ʒ��κ� ������ �κκ��� �߰��� �����ϴ� ������ ����
			vec3& Position);
		
		struct HitInfo
		{
			OBJECT_ID::EID _ID{};
			OBJECT_TAG::ETAG _TAG{};
			// �븻
			vec3 Normal{} ;
			// �ε��� ��ü�� ���� �ٶ󺸴� ����
			vec3 Dir{} ;
			// �ε��� ��ü�� �����ϴ� ���� ��ŭ�� �Ÿ�
			float Distance{} ;
			// �ε��� ��ü�� �߽���
			vec3 Position{} ;
			// Ÿ��
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
