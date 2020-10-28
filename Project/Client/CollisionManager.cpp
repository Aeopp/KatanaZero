#include "stdafx.h"
#include "CollisionManager.h"
#include "CollisionLineManager.h"
#include "CollisionTileManager.h"
#include "CollisionComponent.h"
#include "ComponentManager.h"

void CollisionManager::Update()
{
	CollisionTileManager::instance().Update();
	CollisionLineManager::instance().Update();

	auto& CollisionCompVec = ComponentManager::instance().Find<CollisionComponent>();

	for (auto& _CollisionComp : CollisionCompVec)
	{
		if (_CollisionComp->_Control.bUpdate)
			_CollisionComp->_Control._Update(*_CollisionComp);
		else
			_CollisionComp->Update();
	}

	for (auto& _CollisionCompLhs : CollisionCompVec)
	{
		for (auto& _CollisionCompRhs : CollisionCompVec)
		{
			if (_CollisionCompLhs != _CollisionCompRhs)
			{
				if (!_CollisionCompLhs->bCollision || !_CollisionCompRhs->bCollision)continue;
				// 태그 타입 매칭 검사
				auto IsFind = _TagMatchMap.find(_CollisionCompLhs->_Tag);
				if (IsFind == end(_TagMatchMap))continue;
				if (!IsFind->second.contains(_CollisionCompRhs->_Tag))continue;
				// // 

				auto LhsRect =_CollisionCompLhs->GetWorldRectPt();
				auto RhsRect = _CollisionCompRhs->GetWorldRectPt();

				auto oToRhs = math::Collision::RectAndRect({ LhsRect, RhsRect }, false);

				if (oToRhs)
				{
					auto LhsOwner = _CollisionCompLhs->_Owner.lock();
					auto RhsOwner = _CollisionCompRhs->_Owner.lock();

					// GetNormal From RhsRect
					math::Collision::HitInfo _RhsHitInfo;
					vec3 Normal = *oToRhs;

					if (std::abs(Normal.x) < std::abs(Normal.y))
						Normal.y = 0;
					else
						Normal.x = 0;

					D3DXVec3Normalize(&Normal, &Normal);
					// //
					vec3 Dis = RhsOwner->_TransformComp->Position - LhsOwner->_TransformComp->Position;
					vec3 Dir; 
					D3DXVec3Normalize(&Dir, &Dis);
					D3DXVec3Normalize(&*oToRhs, &*oToRhs);

					_RhsHitInfo.PosDir = Dir;
					_RhsHitInfo.PosDistance = D3DXVec3Length(&Dis);
					 
					_RhsHitInfo.Dir = *oToRhs;
					_RhsHitInfo.Normal = Normal;
					_RhsHitInfo.IntersectAreaScale = D3DXVec3Length(&*oToRhs);
					_RhsHitInfo.Position = math::GetCenter(LhsRect);
					_RhsHitInfo._ID = LhsOwner->GetID();
					_RhsHitInfo._TAG = LhsOwner->GetTag();
					_RhsHitInfo._Target = LhsOwner;
					_RhsHitInfo.IsLhs = false;
					_RhsHitInfo.PushDir = _CollisionCompLhs->CurrentPushDir;
					_RhsHitInfo.PushForce = _CollisionCompLhs->PushForce;
					RhsOwner->Hit(LhsOwner, _RhsHitInfo);

					auto oToLhs = math::Collision::RectAndRect({ RhsRect,LhsRect  }, false);
					Normal = *oToLhs;
					if (std::abs(Normal.x) < std::abs(Normal.y))
						Normal.y = 0;
					else
						Normal.x = 0;
					D3DXVec3Normalize(&Normal, &Normal);

					math::Collision::HitInfo _LhsHitInfo;
					_LhsHitInfo.Dir=_RhsHitInfo.Dir * -1.f;
					_LhsHitInfo.PosDir = Dir * -1.f;
					_LhsHitInfo.PosDistance = _RhsHitInfo.PosDistance;
					_LhsHitInfo.IntersectAreaScale=_RhsHitInfo.IntersectAreaScale;
					_LhsHitInfo.Normal = Normal;
					_LhsHitInfo.Position = math::GetCenter(RhsRect);
					_LhsHitInfo._ID = RhsOwner->GetID();
					_LhsHitInfo._TAG = LhsOwner->GetTag();
					_LhsHitInfo._Target = RhsOwner;
					_LhsHitInfo.IsLhs = true;
					_LhsHitInfo.PushForce = _CollisionCompRhs->PushForce;
					_LhsHitInfo.PushDir = _CollisionCompRhs->CurrentPushDir;
					LhsOwner->Hit(RhsOwner, std::move(_LhsHitInfo ) );
				}
			}
		}
	}
}

void CollisionManager::Initialize()
{
	CollisionTileManager::instance().Initialize();
}

void CollisionManager::Render()
{
	CollisionTileManager::instance().DebugRender();
	CollisionLineManager::instance().DebugRender();

	auto& CollisionCompVec = ComponentManager::instance().Find<CollisionComponent>();

	for (auto& _CollisionComp : CollisionCompVec)
	{
		if (_CollisionComp->_Control.bRender)
			_CollisionComp->_Control._Render(*_CollisionComp);
		else
			_CollisionComp->Render();
	}
}

void CollisionManager::LateUpdate()
{
	CollisionTileManager::instance().LateUpdate();
	CollisionLineManager::instance().LateUpdate();
}
