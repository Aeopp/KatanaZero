#include "stdafx.h"
#include "ObjectManager.h"
#include "object.h"

void ObjectManager::Update()
{

	for (auto& [Tag, ID_ObjVec] : _ObjectMap)
	{
		for (auto& [ID, ObjVec] : ID_ObjVec)
		{
			for (size_t Idx = 0; Idx < ObjVec.size(); ++Idx)
			{
				auto& sp_Obj = ObjVec[Idx];

				if (sp_Obj->bDie || !sp_Obj)
				{
					sp_Obj = std::move(ObjVec.back());
					ObjVec.pop_back();
				}
				else 
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
			for (size_t Idx = 0; Idx < ObjVec.size(); ++Idx)
			{
				auto& sp_Obj = ObjVec[Idx];

				if (sp_Obj->bDie || !sp_Obj)
				{
					sp_Obj = std::move(ObjVec.back());
					ObjVec.pop_back();
				}
				else 
					sp_Obj->LateUpdate();
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

void ObjectManager::Clear() & noexcept
{
	_ObjectMap.clear();
}




