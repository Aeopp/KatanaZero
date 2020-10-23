#include "stdafx.h"
#include "object.h"
#include "ComponentManager.h"

void object::SetOwner(std::weak_ptr<class object> _Owner)
{
	bOwner = !_Owner.expired();
	this->_Owner = _Owner;
}

bool object::IsValid(const std::weak_ptr<class object>& _Check)
{
	return !_Check.expired(); 
}

void object::Initialize() & noexcept
{

}

void object::LateInitialize() & noexcept
{

}

void object::Release() & noexcept
{

}

void object::Update()
{

}

void object::LateUpdate()
{
	if (bOwner)
	{
		auto spOwner = _Owner.lock();
		if (!spOwner) bDie = true;
	}
}

void object::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{

}; 

void object::MapHit(math::Collision::HitInfo _CollisionInfo)
{

};





