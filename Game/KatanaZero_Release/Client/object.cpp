#include "stdafx.h"
#include "object.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"

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
	//bSmoke = false;

	
}

		
void object::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{

}; 

void object::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{

};


void object::LineLanding(const vec3 LineDir)
{
	_TransformComp->bLineMode = true;
	_TransformComp->CurrentLineDir = LineDir;
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;
	_Physic_TransformComp->FlyEnd();
};

void object::LineOff()
{
	_TransformComp->bLineMode = false; 
};





