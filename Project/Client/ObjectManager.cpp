#include "stdafx.h"
#include "ObjectManager.h"
#include "object.h"

void ObjectManager::Update()
{
	for (auto& [Tag, ID_ObjVec] : _ObjectMap)
	{
		for (auto& [ID, ObjVec] : ID_ObjVec)
		{
			for (auto obj_iter = std::begin(ObjVec); obj_iter != std::end(ObjVec);
				++obj_iter)
			{
				auto sp_Obj = *obj_iter;
				if (!sp_Obj)continue;
				sp_Obj->Update();
			}
		}
	}
}

void ObjectManager::LateUpdate()
{
	for (auto& [Tag, ID_ObjVec] : _ObjectMap)
	{
		for (auto& [ID, ObjVec] : ID_ObjVec)
		{
			for (auto obj_iter = std::begin(ObjVec); obj_iter != std::end(ObjVec);)
			{
				auto sp_Obj = *obj_iter;
				if (!sp_Obj)continue;
				sp_Obj->LateUpdate();
				if (sp_Obj->bDie)
				{
					sp_Obj = std::move(ObjVec.back());
					ObjVec.pop_back();
				}

				++obj_iter;
			}
		}
	}
}

void ObjectManager::Initialize() & noexcept
{
	
}

void ObjectManager::Release() & noexcept
{
}




