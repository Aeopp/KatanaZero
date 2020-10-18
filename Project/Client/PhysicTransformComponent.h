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
	// ���� , �� �ð� , ������ , ��������
	void Push(vec3 Vec, vec3 RotationVec,const float Coefficient, const int32_t Exponent);
	std::vector<SimplePhysics> Forces;
	virtual void Update()override;;
	virtual void LateUpdate()override;;
};

