#include "stdafx.h"
#include "PhysicTransformComponent.h"
#include "Time.h"
#include "math.h"
// �ӵ� = ���ۼӵ� + (���ӵ�*�ð�)

// ���ӵ��� �ӵ��� �ݿ�
// Speed += (Acceleration*dt)

// �߷��� �ݿ��Ѵ�.
// Speed += (G * dt);

// �ӵ��� ������ �ݿ�
// Speed *= Friction 

// �ӵ��� ��ǥ�� �ݿ�
// Pos += Speed *dt

void PhysicTransformComponent::Move(SimplePhysics _Physics)
{
	Forces.emplace_back(std::move(_Physics));
};

void PhysicTransformComponent::Flying()
{
	bFly = true;
	bLand = false;
	bDownLand = false;
};

void PhysicTransformComponent::Landing()
{
	bLand = true;
	bFly = false;
	GravityAcceleration = 0.f;
}
void PhysicTransformComponent::Reflect(const float Repulsion)
{
	//for (auto& _Physics : Forces)
	//{
	//	// ������ �����ϱ�
	//	_Physics.
	//}
};

void PhysicTransformComponent::Update()
{
	TransformComponent::Update();

	const float Dt = Time::instance().Delta();

	GravityAcceleration += (math::GRAVITY * Dt);

	for (auto iter = std::begin(Forces); std::end(Forces) != iter;)
	{
		auto& _Physics=*iter;

		_Physics.T += Dt;
		_Physics.Speed += (_Physics.Dir * (_Physics.Acceleration * Dt));
		_Physics.Speed *= (_Physics.Friction );

		vec3 RepulsionVec = (((-_Physics.Speed) * (_Physics.Resistance)));

		float Force = D3DXVec3Length(&_Physics.Speed);
		float RepulsionForce= D3DXVec3Length(&RepulsionVec);

		Position += _Physics.Speed * Dt;
		Position += RepulsionVec*Dt;
		if (_Physics.T > _Physics.MaxT  || std::abs(Force- RepulsionForce )< Mass)
			iter =Forces.erase(iter); 
		else 
			++iter;
	}

	Position += vec3{ 0.f,1.f,0.f }*GravityAcceleration * Dt;
}

void PhysicTransformComponent::LateUpdate()
{
	TransformComponent::LateUpdate();
}
