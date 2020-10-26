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
	virtual void Move(vec3 Dir, const float AddSpeed)override; 
public : 
	void AttackSlash();
	void TimeRegist();
	void KeyBinding() & noexcept;
public : 
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
	void HurtGround();
	void HurtGroundState();
	void HurtRecover();
	void HurtRecoverState();

	void AnyState();
	// // 
	void FSM();
private:
	float CurAttackCoolTime = 0.0f;
public  : 
	bool bCurWallRideCollision{ false };
private :
	bool bJumpAnimEnd{ false };
	bool bIdleToRunMotionEnd{ false };
	bool bRunToIdleMotionEnd{ false };
	bool bAttackMotionEnd{ false };
	bool bRollMotionEnd{ false };
	bool bFlipMotionEnd{ false };
	bool bDoorKickMotionEnd{ false };
	bool bPreCrouchMotionEnd{ false };
	bool bPostCrouchMotionEnd{ false };
	
	bool bSneakKeyCheck{ false };
	bool bJumpKeyCheck{ false };
	bool bMoveKeyCheck{ false };
	bool bAttackKeyCheck{ false };
	bool bDownKeyCheck{ false };

	bool bFrameCurrentCharacterInput = false;
public :
	bool bWallJump{ false };

	vec3 FlipDir{0.f,0.f,0.f};
	void JumpWallRide();
	std::shared_ptr<class PhysicTransformComponent>_PhysicComp;
private:
	const float PlayerSpeed = 1500.f;
private:
	std::shared_ptr<class Camera> _SpCamera{};
	std::shared_ptr<class Battery> _SpBattery{};
	std::shared_ptr<class UIItemIcon > _SpUIItemIcon{};
};

