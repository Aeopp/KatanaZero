#include "stdafx.h"
#include "PhysicTransformComponent.h"
#include "Time.h"
#include "math.h"
// 속도 = 시작속도 + (가속도*시간)

// 가속도를 속도에 반영
// Speed += (Acceleration*dt)

// 중력을 반영한다.
// Speed += (G * dt);

// 속도에 마찰을 반영
// Speed *= Friction 

// 속도를 좌표에 반영
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
}

void PhysicTransformComponent::FlyEnd()
{
	bFly = false;
	bLand = true;
};

void PhysicTransformComponent::Landing()
{
	bLand = true;
	bFly = false;
	GravityAcceleration = 0.f;
}
void PhysicTransformComponent::DownLanding()
{
	Landing();
	bDownLand = true;
}
void PhysicTransformComponent::Reflect(const float Repulsion)
{
	//for (auto& _Physics : Forces)
	//{
	//	// 방향을 조정하기
	//	_Physics.
	//}
};

void PhysicTransformComponent::Update()
{
	TransformComponent::Update();

	const float Dt = Time::instance().Delta();

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
		Position +=  RepulsionVec*Dt;
		if (_Physics.T > _Physics.MaxT || std::abs(Force - RepulsionForce) < Mass * Time::instance().TimeScale)
		{
			iter = Forces.erase(iter);
			continue;
		}
			++iter;
	}

	if (bGravity)
	{
		GravityAcceleration += (GravityCoefficient * (math::GRAVITY * Dt));
		Position += vec3{ 0.f,1.f,0.f }*GravityAcceleration * Dt;
	}
}

void PhysicTransformComponent::LateUpdate()
{
	TransformComponent::LateUpdate();
}
