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
}
void PhysicTransformComponent::Move(
	vec3 InitVec, float Acceleration, float MaxT, vec3 Dir, float InitT)
{
	if (Dir == vec3{ 0.f,0.f,0.f })
	{
		D3DXVec3Normalize(&Dir, &InitVec);
	};

	Forces.emplace_back(SimplePhysics{ InitVec,Acceleration,Dir,InitT,MaxT });
}
;

void PhysicTransformComponent::Flying()
{
	bFly = true;
	bLand = false;
	bDownLand = false;
};

void PhysicTransformComponent::FlyEnd()
{
	bFly = false;
	bLand = true;
};

void PhysicTransformComponent::Landing()
{
	bLand = true;
	bFly = false;
	bCurCollisionTile = true;
	GravityAcceleration = 0.f;
};

void PhysicTransformComponent::DownLanding()
{
	Landing();
	bDownLand = true;
};

void PhysicTransformComponent::Reflect(const float Repulsion)
{
	//for (auto& _Physics : Forces)
	//{
	//	// 방향을 조정하기
	//	_Physics.
	//}
};

void PhysicTransformComponent::ForceClear()
{
	Forces.clear();
}

void PhysicTransformComponent::Update()
{
	TransformComponent::Update();

	const float Dt = Time::instance().Delta();

	for (auto iter = std::begin(Forces); std::end(Forces) != iter;)
	{
		auto& _Physics=*iter;
		_Physics.T += Dt;
		_Physics.Speed += (_Physics.Dir * (_Physics.Acceleration * Dt));

		float Force = D3DXVec3Length(&_Physics.Speed);

		Position += _Physics.Speed * Dt;

		if (_Physics.T >=_Physics.MaxT || std::abs(Force) < Mass)
		{
			iter = Forces.erase(iter);
			continue;
		}
			++iter;
	}

	if (bGravity)
	{
		GravityAcceleration += (math::GRAVITY_ACC * Dt);
		Position += vec3{ 0.f,1.f,0.f }* ( math::GRAVITY + GravityAcceleration) * GravityCoefficient * Dt;
	}
}

void PhysicTransformComponent::LateUpdate()
{
	TransformComponent::LateUpdate();



}
