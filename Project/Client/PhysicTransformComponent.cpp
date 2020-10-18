#include "stdafx.h"
#include "PhysicTransformComponent.h"
#include "Time.h"

void PhysicTransformComponent::Push(vec3 Vec, vec3 RotationVec, const float Coefficient, const int32_t Exponent)
{
	Forces.emplace_back(SimplePhysics{ Vec,RotationVec,Coefficient,Exponent } );
}


void PhysicTransformComponent::Update()
{
	TransformComponent::Update();

	const float Dt = Time::instance().Delta();

	for (auto iter = std::begin(Forces); std::end(Forces) != iter; ++iter)
	{
		auto& [Vec, RVec, Coefficient, Exponent] = *iter;
		Position += Vec * Dt;
		Rotation += RVec * Dt;

		Vec.x-=std::pow(Vec.x, Exponent)* Coefficient;
		Vec.y-=std::pow(Vec.y, Exponent)* Coefficient;

		RVec.x -= std::pow(RVec.x, Exponent) * Coefficient;
		RVec.y -= std::pow(RVec.y, Exponent) * Coefficient;
		static constexpr float Min = 0.04f;

		if ((Vec.x + Vec.y + RVec.x + RVec.y) <Min)
		{
			*iter = std::move(Forces.back());
			Forces.pop_back();
		}
	}
}

void PhysicTransformComponent::LateUpdate()
{
	TransformComponent::LateUpdate();
}
