#pragma once
#include "Character.h"
#include <bitset>

class Player  : public Character
{
public :
	enum class State : uint8_t
	{
		Attack,
		Crouch,
		Dash,
		DoorKick,
		Fall,
		Flip,
		HurtFly,
		HurtFly_Begin,
		Hurt_Ground,
		HurtRecover,
		Idle,
		Idle_to_Run,
		Idle_to_sneak,
		Jump,
		Post_Crouch,
		Pre_Crouch,
		Roll,
		Run,
		Run_to_Idle,
		Sneak,
		StairFall,
		Wall_Ride,
		DownJump ,
	};
	using Super = Character;
private:
	typename Player::State _CurrentState{ Player::State::Idle };
public :
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const& ;
public:
	virtual void Initialize() & noexcept;
	virtual void LateInitialize() & noexcept;
	virtual void Release() & noexcept;
	virtual void Update();
	virtual void LateUpdate();
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo)override;
	virtual void Hit(std::weak_ptr<class object>_Target, math::Collision::HitInfo _CollisionInfo)override;
	virtual void Move(vec3 Dir, const float AddSpeed)override; 
	bool IsInvisible() { return  _CurrentState == Player::State::Roll || (InvincibleTime > 0.f )  || _CurrentState == Player::State::Dash ||(_CurrentState == Player::State::Flip); };
	void HurtGround();
	bool bHurt{ false };
public:
	std::shared_ptr<class UIItemIcon > _SpUIItemIcon{};
	bool bInAreaSmoke = false;

private:
	void AttackSlash();
	void TimeRegist();
	void KeyBinding() & noexcept;
private:
	//State 
	void Idle(); 
	void IdleState();
	void IdleToRun();
	void IdleToRunState();
	void Run(); 
	void RunState();
	void RunToIdle();
	void RunToIdleState();
	void Fall();
	void FallState();
	void Jump();
	void JumpState();
	void Attack();
	void AttackState();
	void Roll();
	void RollState();
	void WallRide();
	void WallRideState();
	void Flip();
	void FlipState();
	void DownJump();
	void DownJumpState();
	void DoorKick();
	void DoorKickState();
	void PreCrouch();
	void PreCrouchState();
	void Crouch();
	void CrouchState();
	void PostCrouch();
	void PostCrouchState();
	void Sneak();
	void SneakState();
	void HurtFlyBegin();
	void HurtFlyBeginState();
	void HurtFly();
	void HurtFlyState();
	void Dash();
	void DashState();

	void HurtGroundState();
	void HurtRecover();
	void HurtRecoverState();
private:
	void AnyState();
	void FSM();
private :
	bool bJumpMotionEnd{ false };
	bool bIdleToRunMotionEnd{ false };
	bool bRunToIdleMotionEnd{ false };
	bool bAttackMotionEnd{ false };
	bool bRollMotionEnd{ false };
	bool bFlipMotionEnd{ false };
	bool bDoorKickMotionEnd{ false };
	bool bPreCrouchMotionEnd{ false };
	bool bPostCrouchMotionEnd{ false };
	bool bHurtFlyBeginMotionEnd{ false };
	bool bHurtGroundMotionEnd{ false }; 
	bool bHurtRecoverMotionEnd{ false };
	bool bDashEnd{ false };
private:
	bool bFrameCurrentCharacterInput = false;
private:

private:
	bool bAttacking{ false };
	bool bCheat = false;
	bool bSneakKeyCheck{ false };
	bool bJumpKeyCheck{ false };
	bool bMoveKeyCheck{ false };
	bool bAttackKeyCheck{ false };
	bool bDashKeyCheck{ false };
	

	bool bDownKeyCheck{ false };

	vec3 AtDashDir{};
public :	
	
	bool bDashRangeRender = false; 
	bool bFatal = false;
	bool bInit = false;
	bool bSneak{ false };
	vec3 FlipDir{0.f,0.f,0.f};
	vec3 CurWallRideLinePos{ 0,0,0 };
	bool bCurWallRideCollision{ false };
	bool bWallJump{ false };
	float InvincibleTime = 0.f;
	// 테스트끝나고수정
	const float DashCoolTime = 2.f;
	// 
	float CurDashCoolTime = 0.f;
	void JumpWallRide();

	float CurDashRange = 0.f;

	bool IsEqItem()const&;;

	void EqItem(std::shared_ptr<class Item> _Item);
private:
	std::optional<vec3 > AtAttackDir;
	float RollEffectDelta = 0.0025f;
	float WallRideEffectDelta = 0.1f;
	float CurAttackCoolTime = 0.0f;
	const float PlayerSpeed = 1250.f;
	std::shared_ptr<class Attack_Slash> _SpAttackSlash{};
	std::shared_ptr<class Battery> _SpBattery{};
	std::shared_ptr<class Item> _SpCurItem{};

	std::weak_ptr<class object> _CurDoor;

};

