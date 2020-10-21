#include "stdafx.h"
#include "Component.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "Test.h"

void ComponentManager::Initialize() & noexcept
{

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

			spComp->Update();
		}
	}
}

void ComponentManager::LateUpdate()
{
	for (auto OuterIter = std::begin(ComponentMap); OuterIter != std::end(ComponentMap); ++OuterIter)
	{
		if (DoesNotUpdateType.contains(OuterIter->first))continue;

		for (auto spCompIter = std::begin(OuterIter->second); spCompIter != std::end(OuterIter->second); ++spCompIter)
		{
			auto spComp = *spCompIter;

			if (!spComp || spComp->bDie)
			{
				*spCompIter = OuterIter->second.back();
				OuterIter->second.pop_back();
			}
			else 
				spComp->LateUpdate();
		}
	}
}
void ComponentManager::Render()
{
	for (auto OuterIter = std::begin(ComponentMap); OuterIter != std::end(ComponentMap); ++OuterIter)
	{
		if (DoesNotUpdateType.contains(OuterIter->first))continue;

		for (auto spCompIter = std::begin(OuterIter->second); spCompIter != std::end(OuterIter->second); ++spCompIter)
		{
			auto spComp = *spCompIter;

			spComp->Render();
		}
	}
};

