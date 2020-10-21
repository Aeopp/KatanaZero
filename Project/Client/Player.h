#pragma once
#include "object.h"

class Player  : public object
{
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
	
	void Move(const vec3 Dir);

	float Speed = 2000.f;

	std::shared_ptr<class RenderComponent> _RenderComp;
};

