#include "stdafx.h"
#include "Component.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "RenderComponent.h"
#include "UIRenderComponent.h"
#include "CollisionComponent.h"

void ComponentManager::Initialize() & noexcept
{
	DoesNotRenderType.insert(typeid(RenderComponent));
	DoesNotRenderType.insert(typeid(UIRenderComponent));
	DoesNotRenderType.insert(typeid(CollisionComponent));


	DoesNotUpdateType.insert(typeid(CollisionComponent));
}

void ComponentManager::Update()
{
	for (auto OuterIter = std::begin(ComponentMap); OuterIter != std::end(ComponentMap);++OuterIter)
	{
		if (DoesNotUpdateType.contains(OuterIter->first))continue;

		for (auto spCompIter = std::begin(OuterIter->second); spCompIter != std::end(OuterIter->second); ++spCompIter)
		{
			auto spComp = *spCompIter;
			if (!spComp)continue;

			if (spComp->_Control.bUpdate)
			{
				spComp->_Control._Update(*spComp);
			}
			else 
				spComp->Update();
		}
	}
}

void ComponentManager::LateUpdate()
{
	for (auto OuterIter = std::begin(ComponentMap); OuterIter != std::end(ComponentMap); ++OuterIter)
	{
		if (DoesNotLateUpdateType.contains(OuterIter->first))continue;

		for (auto spCompIter = std::begin(OuterIter->second); spCompIter != std::end(OuterIter->second); ++spCompIter)
		{
			auto spComp = *spCompIter;

			if (!spComp || spComp->bDie)
			{
				*spCompIter = OuterIter->second.back();
				OuterIter->second.pop_back();
			}
			else 
			{
				if (spComp->_Control.bLateUpdate)
				{
					spComp->_Control._LateUpdate(*spComp);
				}
				else 
					spComp->LateUpdate();
			}
				
		}
	}
}
void ComponentManager::Render()
{
	for (auto OuterIter = std::begin(ComponentMap); OuterIter != std::end(ComponentMap); ++OuterIter)
	{
		if (DoesNotRenderType.contains(OuterIter->first))continue;

		for (auto spCompIter = std::begin(OuterIter->second); spCompIter != std::end(OuterIter->second); ++spCompIter)
		{
			auto spComp = *spCompIter;

			if (spComp->_Control.bRender)
			{
				spComp->_Control._Render(*spComp);
			}
			else 
				spComp->Render();
		}
	}
};

