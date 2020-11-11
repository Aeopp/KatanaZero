#pragma once
#include "NormalEnemy.h"
class Gangster :
    public NormalEnemy
{
public :
	using Super = NormalEnemy;
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
public:
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
		Whip,
		EnterStair,
		LeaveStair,
		InSmoke,
	};
private:
	//TODO :: ���⼭ ���ð�ü 
//	std::shared_ptr<class Grunt_Slash> _SpAttack;
	std::shared_ptr<class GangsterArm> _SpArm;
	std::shared_ptr<class GangsterGun> _SpGun;
	typename Gangster::State _CurrentState{ Gangster::State::Idle };
	float AttackCoolTime = 0.6f;
public :
	virtual void Initialize() & noexcept;
	virtual void Update();
	virtual void LateUpdate();
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo)override;
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
	virtual void Move(vec3 Dir, const float AddSpeed)override;
	virtual void Die() & override;
public :
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
	void Whip();
	void WhipState();

	void InSmoke();
	void InSmokeState();
	
private:
	void AnyState();
	void FSM();
	bool bSmokeEnd = false;
	float DoorTurnDuration = 1.f;

	void FollowRouteProcedure();

	const float RunSoundTime = 0.5f;
	float CurRunSoundTime = RunSoundTime;
	const float WalkSoundTime = 0.5f;
	float CurWalkSoundTime = WalkSoundTime;
public:
	bool bTurnMotionEnd{ false };
	bool bWhipMotionEnd{ false };

	float GunRotZ = 0.f;
	float _Y = 0.f;
	// NormalEnemy��(��) ���� ��ӵ�
	virtual void SetUpInitState(float DirX, int32_t StateID) override;
};

