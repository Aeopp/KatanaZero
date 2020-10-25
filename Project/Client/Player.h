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

	//State 
	void Idle();
	void IdleToRun();
	void Run();
	void RunToIdle();
	void Fall();
	void WallRide();
	void Jump();
	// // 
	void FSM();

	void MoveStart(const vec3 Dir);
	void MoveEnd(const vec3 Dir);

	void KeyBinding() & noexcept;
	
	void DownJump();
	void Attack();
	void AttackSlash();

	void Roll();

	void TimeRegist();
	void PreCrouch();
	void Crouch();
	void PostCrouch();
	
	void WallRideEnd();
	bool bWallRide{ false };
private :
	bool bJumpAnimEnd{ false };
	bool bJumpKeyCheck{ false };
	bool bMoveCurrentFrameKeyCheck{ false };
	bool bIdleToRunMotionEnd{ false };
	bool bRunToIdleMotionEnd{ false };
	bool bAttackMotionEnd{ false }; 
public :
	
	bool bWallJump{ false };
	bool bMovingOn{ false };
	bool bJumpStartAnimEnd{ false };
	bool bSneak{ false };

	bool CurrentFrameCharacterInput = false;

	vec3 WallRideDir{0.f,0.f,0.f};
	float GravityRepulsion = 100.f;

	bool bCrouch = false;

	const float RollCoolTime = 0.25f;
	float CurrentRollCoolTime = 0.0f;
	float RollSpeed = 1000.f;
	
	const float AttackForce = 500.f;
	const float AttackAcceleration = 200.f;
	

	const float JumpCoolTime = 0.3f;
	float CurrentJumpCoolTime = 0.0f;

	const float WallJumpCoolTime = 0.3f;
	float CurrentWallJumpCoolTime = 0.0f;
	
	const float PlayerSpeed = 2000.f;

	const float AnimFrameSpeed = 0.085f;

	void JumpWallRide();

	std::bitset<2> _RollCheck;
	vec3 RollDir{ 1.f,0.f,0.f };
private:
	vec3 CurrentMoveDir{ 1.f,0.f,0.f };
	std::shared_ptr<class PhysicTransformComponent>_PhysicComp;
private:
	std::shared_ptr<class Camera> _SpCamera{};
	std::shared_ptr<class Battery> _SpBattery{};
	std::shared_ptr<class UIItemIcon > _SpUIItemIcon{};
};

