#pragma once
#include "NormalEnemy.h"

class Grunt :
    public NormalEnemy
{
public :
	using Super = NormalEnemy;
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
public :
	enum class State : uint8_t
	{
		Attack,
		Fall,
		HurtFly,
		HurtGround,
		Idle,
		Run,
		Turn,
		Walk,
		EnterStair,
		LeaveStair,
		InSmoke,
	};
private:
	std::shared_ptr<class Grunt_Slash> _SpAttack;
	typename Grunt::State _CurrentState{ Grunt::State::Idle };
public :
	virtual void Initialize() & noexcept;
	virtual void Update();
	virtual void LateUpdate();
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo)override;
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
	virtual void Move(vec3 Dir, const float AddSpeed)override;
	virtual void Die() & override;
public:
	void EnterStair();
	void LeaveStair();
	void EnterStairState();
	void LeaveStairState();
	void Attack();
	void AttackState();
	void Fall();
	void FallState();
	void HurtFly();
	void HurtFlyState();
	void HurtGround();
	void HurtGroundState();
	void Idle();
	void IdleState();
	void Run();
	void RunState();
	void Turn()override;
	void TurnState();
	void Walk();
	void WalkState();

	void InSmoke();
	void InSmokeState();
private:
	void AnyState();
	void FSM();

	void FollowRouteProcedure();
private:
	int32_t SmokeCount = 0;
public:
	float DoorTurnDuration = 1.f;

	bool bAttackMotionEnd{ false };
	bool bTurnMotionEnd{ false };
	bool bSmokeEnd{ false };

	float _Y = 0.f;
	// NormalEnemy을(를) 통해 상속됨
	virtual void SetUpInitState(float DirX, int32_t StateID) override;
};

