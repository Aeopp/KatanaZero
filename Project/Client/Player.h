#pragma once
#include "Character.h"

class Player  : public Character
{
public :
	using Super = Character;
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

	void KeyBinding()&noexcept;
	void Jump();
	void DownJump();

	void TimeRegist();
	
	void WallRideEnd();
	void WallRide();
	bool bWallRide{ false };
	bool bWallJump{ false };

	vec3 WallRideDir{0.f,0.f,0.f};
	float GravityRepulsion = 100.f;

	const float JumpCoolTime = 0.3f;
	const float WallJumpCoolTime = 0.3f;

	float CurrentJumpCoolTime = 0.0f; 
	float CurrentWallJumpCoolTime = 0.0f;
	const float PlayerSpeed = 1000.f;

	void JumpWallRide();
private:
	std::shared_ptr<class Camera> _SpCamera{};
	std::shared_ptr<class Battery> _SpBattery{};
	std::shared_ptr<class UIItemIcon > _SpUIItemIcon{};
};

