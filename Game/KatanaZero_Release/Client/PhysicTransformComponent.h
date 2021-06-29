#pragma once
#include "TransformComponent.h"
// 마찰은 물체와 충돌했을때에만 발생하는것에 유의

// 속도 = 시작속도 + (가속도*시간)

// 가속도를 속도에 반영
// Speed += (Acceleration*dt)

// 중력을 반영한다.
// Speed += (G * dt);

// 속도에 마찰을 반영
// Speed *= Friction 

// 속도를 좌표에 반영
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
	//            방향        회전값                  계수                     지수
	void Move(SimplePhysics _Physics);
	void Move(vec3 InitVec, float Acceleration, float MaxT, 
	vec3 Dir = { 0.f,0.f,0.f } , float InitT = 0.f);
	void Flying();
	void FlyEnd();
	void Landing();
	void DownLanding();
						// 반발력
	void Reflect(const float Repulsion);
	// 땅에 닿았다는 처리가 발생하면 초기화 해주세요.
	float GravityAcceleration = 0.f;
	float GravityCoefficient = 1.f;

	float Friction = 1.f;
	float Mass = 100.f;
	bool bLand = false;
	bool bFly = false;
	bool bDownLand = false;
	bool bGravity = false;
	bool bCurCollisionTile = false;

	void ForceClear();

	std::vector<SimplePhysics> Forces;
	virtual void Update()override;;
	virtual void LateUpdate()override;;
};

