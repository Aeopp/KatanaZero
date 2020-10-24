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

	virtual void Move(vec3 Dir, const float AddSpeed)override; 

	void KeyBinding()&noexcept;
	void Jump();
	void DownJump();
	
	
private:
	std::shared_ptr<class Camera> _SpCamera{};
	std::shared_ptr<class Battery> _SpBattery{};
	std::shared_ptr<class UIItemIcon > _SpUIItemIcon{};
};

