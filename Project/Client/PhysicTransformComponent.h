#pragma once
#include "TransformComponent.h"
// ������ ��ü�� �浹���������� �߻��ϴ°Ϳ� ����

// �ӵ� = ���ۼӵ� + (���ӵ�*�ð�)

// ���ӵ��� �ӵ��� �ݿ�
// Speed += (Acceleration*dt)

// �߷��� �ݿ��Ѵ�.
// Speed += (G * dt);

// �ӵ��� ������ �ݿ�
// Speed *= Friction 

// �ӵ��� ��ǥ�� �ݿ�
// Pos += Speed *dt

struct SimplePhysics
{
	vec3 Speed{ 0.f,0.f,0.f };
	float Acceleration{ 0.f };
	vec3 Dir{ 0.f,0.f,0.f };
	float T = 0.f;
	float MaxT = 0.f;
};

class PhysicTransformComponent : public TransformComponent
{
public:
	//            ����        ȸ����                  ���                     ����
	void Move(SimplePhysics _Physics);
	void Flying();
	void FlyEnd();
	void Landing();
	void DownLanding();
						// �ݹ߷�
	void Reflect(const float Repulsion);
	// ���� ��Ҵٴ� ó���� �߻��ϸ� �ʱ�ȭ ���ּ���.
	float GravityAcceleration = 0.f;
	float GravityCoefficient = 1.f;

	float Friction = 1.f;
	float Mass = 100.f;
	bool bLand = false;
	bool bFly = false;
	bool bDownLand = false;
	bool bGravity = true;
	std::vector<SimplePhysics> Forces;
	virtual void Update()override;;
	virtual void LateUpdate()override;;
};

