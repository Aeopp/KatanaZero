#pragma once
#include "TransformComponent.h"

struct SimplePhysics
{
	vec3 Vec, RotationVec;
	float Coefficient;
	int32_t Exponent;
};

class PhysicTransformComponent : public TransformComponent
{
public:
	// 방향 , 총 시간 , 방향계수 , 방향지수
	void Push(vec3 Vec, vec3 RotationVec,const float Coefficient, const int32_t Exponent);
	std::vector<SimplePhysics> Forces;
public:
	 void Update()override;
	 void LateUpdate()override;
};

