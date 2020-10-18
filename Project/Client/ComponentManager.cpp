#include "stdafx.h"
#include "Component.h"

#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "Test.h"


void ComponentManager::Update()
{
	auto& _TransformContainer = Find<TransformComponent>();

	for (auto iter =  std::begin( _TransformContainer) ;std::end(_TransformContainer)!=iter;++iter)
	{
		auto _TransformComp = *iter;

		if (!_TransformComp||_TransformComp->_Owner.expired())
		{
			*iter =_TransformContainer.back();
			_TransformContainer.pop_back();
			continue;
		}
		else 
			_TransformComp->Update();
	}

	auto& _PhysicTransformContainer = Find<PhysicTransformComponent>();

	for (auto iter = std::begin(_PhysicTransformContainer); std::end(_PhysicTransformContainer) != iter; ++iter)
	{
		auto _PhysicTransformComp = *iter;

		if (!_PhysicTransformComp || _PhysicTransformComp->_Owner.expired())
		{
			_PhysicTransformComp = _PhysicTransformContainer.back();
			_PhysicTransformContainer.pop_back();
			continue;
		}
		else
			_PhysicTransformComp->Update();
	}
}

void ComponentManager::LateUpdate()
{
	auto& _TransformContainer = Find<TransformComponent>();

	for (auto iter = std::begin(_TransformContainer); std::end(_TransformContainer) != iter; ++iter)
	{
		auto _TransformComp = *iter;

		if (!_TransformComp || _TransformComp->_Owner.expired())
		{
			*iter = _TransformContainer.back();
			_TransformContainer.pop_back();
			continue;
		}
		else
			_TransformComp->LateUpdate();
	}

	auto& _PhysicTransformContainer = Find<PhysicTransformComponent>();

	for (auto iter = std::begin(_PhysicTransformContainer); std::end(_PhysicTransformContainer) != iter; ++iter)
	{
		auto _PhysicTransformComp = *iter;

		if (!_PhysicTransformComp || _PhysicTransformComp->_Owner.expired())
		{
			_PhysicTransformComp = _PhysicTransformContainer.back();
			_PhysicTransformContainer.pop_back();
			continue;
		}
		else
			_PhysicTransformComp->LateUpdate();
	}
};

