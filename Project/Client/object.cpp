#include "stdafx.h"
#include "object.h"
#include "ComponentManager.h"

void object::SetOwner(std::weak_ptr<class object> _Owner)
{
	bOwner = !_Owner.expired();
	this->_Owner = _Owner;
}

void object::Initialize() & noexcept
{
	_TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
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




