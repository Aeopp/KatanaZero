#include "stdafx.h"
#include "GraphicDevice.h"
#include "CollisionTileManager.h"
#include "math.h"
#include "PhysicTransformComponent.h"
#include <fstream>
#include <ostream>
#include <istream>
#include "CollisionComponent.h"
#include "ComponentManager.h"
#include "Player.h"
#include "InputManager.h"
#include "Time.h"

void CollisionTileManager::Collision()&
{

}

void CollisionTileManager::Initialize() & noexcept
{
	

}

void CollisionTileManager::Push(const vec3 & Position, const bool bCanGoDown) &
{
	float SizeXHalf = global::TileSize.first / 2;
	float SizeYHalf = global::TileSize.second / 2;

	float CollisionTileX = std::floor(Position.x / global::TileSize.first) 
		*  global::TileSize.first + SizeXHalf;

	float CollisionTileY = std::floor(Position.y / global::TileSize.second) * 
		global::TileSize.second + SizeYHalf;

	_CollisionTileType _Points;

	_Points[0] = { CollisionTileX - SizeXHalf , CollisionTileY - SizeYHalf, 0.f };
	_Points[1] = { CollisionTileX + SizeXHalf , CollisionTileY - SizeYHalf, 0.f };
	_Points[2] = { CollisionTileX + SizeXHalf , CollisionTileY + SizeYHalf, 0.f };
	_Points[3] = { CollisionTileX - SizeXHalf , CollisionTileY + SizeYHalf, 0.f };
	
	const vec3 InsertPoint = { CollisionTileX,CollisionTileY,0.f };

	auto IsContain = [InsertPoint](const auto& TilePoints)
	{
		const float x=  TilePoints[0].x + (TilePoints[2].x - TilePoints[0].x) / 2.f;
		const float y = TilePoints[0].y + (TilePoints[2].y - TilePoints[0].y) / 2.f;

		const vec3 CurrentTileCenter{ x,y,0.f };

		return InsertPoint == CurrentTileCenter;
	};

	auto& CurrentTilePointsVec = GetCollisionTileContainerRef(bCanGoDown);

	auto IsFindIter = std::find_if(std::begin(CurrentTilePointsVec),
		std::end(CurrentTilePointsVec),
		std::move(IsContain));

	bool IsInsertable = (IsFindIter == std::end(CurrentTilePointsVec));

	if (IsInsertable)
		CurrentTilePointsVec.push_back(std::move(_Points));
}

void CollisionTileManager::Erase(const vec3 & TargetPosition, const bool bCanGoDown) &
{
	auto& CurrentTilePointsVec = GetCollisionTileContainerRef(bCanGoDown);

	 auto IsFindIter = std::find_if(std::begin
		(CurrentTilePointsVec),std::end(CurrentTilePointsVec),
		[TargetPosition](const auto& CurrentTilePoints) 
		{
			return math::IsPointInnerRect(CurrentTilePoints, TargetPosition);
		});

	if ( IsFindIter !=std::end(CurrentTilePointsVec))
		CurrentTilePointsVec.erase(IsFindIter);
}

void CollisionTileManager::DebugRender()&
{
	if (!global::bDebug)return;

	static constexpr float DebugLineWidth = 2.f;

	std::pair<float, float > CameraPos{ 0.f,0.f }; 
	float JoomScale = global::JoomScale;

	matrix MJoom;
	CameraPos.first = global::CameraPos.x;
	CameraPos.second = global::CameraPos.y;

	#ifdef _AFX
		CMainFrame*pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		if (!pMain)return;
		CMFCToolView*pView = dynamic_cast<CMFCToolView*>(pMain->_MainSplitter.GetPane(0, 1));
		if (!pView)return;

		CameraPos.first = pView->GetScrollPos(0);
		CameraPos.second = pView->GetScrollPos(1);

		JoomScale = pView->JoomScale;
	#endif

	GraphicDevice::instance().GetSprite()->End();
	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);

	int32_t RenderCount = 0;
	int32_t TileCount = 0;
	MJoom = math::GetCameraJoomMatrix(JoomScale,
		vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

	std::apply([CameraPos, MJoom,&RenderCount,&TileCount](const auto&... _CollisionTileContainer) {

		auto RenderTile = [CameraPos, MJoom, &RenderCount, &TileCount](const _CollisionTileContainerType& _CollisionTileContainer)
		{
			static int32_t RenderColorIDX = 0;
			RenderColorIDX = (RenderColorIDX + 1) % 2;
			static constexpr std::array<D3DCOLOR, std::tuple_size_v<typename CollisionTileManager::_CollisionTileMapType::mapped_type>> ColorTable
			{
				D3DCOLOR_ARGB(255, 255, 255, 0),
				D3DCOLOR_ARGB(255, 255, 0, 0)
			};

			TileCount += _CollisionTileContainer.size();

			for (const auto& CollisionTilePoints : _CollisionTileContainer)
			{
				bool IsRenderable = false;
				std::array <vec2,5ul> CollisionTilePoints2D;
				// Convert 3D->2D
				for (size_t i = 0; i < 4ul; ++i)
				{
					CollisionTilePoints2D[i] = { CollisionTilePoints[i].x - CameraPos.first ,
												CollisionTilePoints[i].y - CameraPos.second };

					vec3 Point3D = { CollisionTilePoints2D[i].x, CollisionTilePoints2D[i].y, 0.f };
					D3DXVec3TransformCoord(&Point3D, &Point3D, &MJoom);
					CollisionTilePoints2D[i].x = Point3D.x;
					CollisionTilePoints2D[i].y = Point3D.y;
					IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), Point3D);
				}
				CollisionTilePoints2D.back() = CollisionTilePoints2D.front();
				if (IsRenderable)
				{
					++RenderCount;

					GraphicDevice::instance().GetLine()->Draw(CollisionTilePoints2D.data(),
						CollisionTilePoints2D.size(), (ColorTable[RenderColorIDX]));
				}
			}
		};

		(RenderTile(_CollisionTileContainer),...);

	}, _CollisionTilePointsMap[CurrentStateKey]);

	std::wstring DebugTileStr = L"RenderDebugCollisionTile : " + std::to_wstring(RenderCount);
	RECT rectRender{ 0,50,500,75 };
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	DebugTileStr = L"CullingDebugCollisionTile : " + std::to_wstring(TileCount - RenderCount);
	rectRender = { 0,75,500,100};
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void CollisionTileManager::Update()&
{
	auto & _CollisionCompVec = ComponentManager::instance().Find<CollisionComponent>();

	// 아래방향 점프가 불가능한 타일
	auto& _CollisionTileVec =GetCollisionTileContainerRef(false);

	for (auto& _spCollision : _CollisionCompVec)
	{
		bool bCollision = false;
		bool bLand = false;
		bool bFly = false;

		if (!_spCollision->bCollision)continue;
		if (!(_CollisionTagSet.contains(_spCollision->_Tag)))continue;
		for (auto& _CollisionTile : _CollisionTileVec)
		{
			auto spOwner = _spCollision->_Owner.lock();
			vec3 CompOwnerPos = spOwner->_TransformComp->Position;
			vec3 CollisionLeftTop = _CollisionTile.front();
			vec3 ToLT = CompOwnerPos - CollisionLeftTop;
			float DistanceSq = D3DXVec3LengthSq(&ToLT);
			// 거리 제곱이 기준치보다 높아서 충돌 검출 무효
			// 거리는 제곱근이아닌 제곱으로하기
			if (DistanceSq > TileCollisionCheckDistanceMinSquare)continue;
			

			auto WorldRectPt = _spCollision->GetWorldRectPt();
			
			auto opDir = math::Collision::RectAndRect({ WorldRectPt, _CollisionTile },false);

			if (opDir)
			{
				bCollision = true;

				 vec3 TilePushDir = *opDir;

				
				if ( std::abs(TilePushDir.x) < std::abs(TilePushDir.y))
					TilePushDir.y = 0;
				else		   
					TilePushDir.x = 0;
				
				if (spOwner->_TransformComp->bMapSlide)
				{
					spOwner->_TransformComp->Position += TilePushDir;
				}

				math::Collision::HitInfo _HitInfo{};
				_HitInfo.PosDistance = D3DXVec3Length(&(*opDir));
				D3DXVec3Normalize(&(*opDir), &(*opDir));
				_HitInfo.PosDir = *opDir;
				D3DXVec3Normalize(&TilePushDir, &TilePushDir);
				_HitInfo.Normal = TilePushDir;
				_HitInfo.Position = math::GetCenter(_CollisionTile);
				_HitInfo._Target = { };
				_HitInfo._ID = OBJECT_ID::ETILE;
				_HitInfo._TAG = OBJECT_TAG::ETERRAIN;

			
				//밀어낸 이후에 위에 존재한다면 땅에닿았었다는 처리
				if (     std::abs ( WorldRectPt[2].y -  _CollisionTile[0].y )  < LandCheckDistance)
				{
					bLand = true;
					auto spPhysicTransform = std::dynamic_pointer_cast<PhysicTransformComponent>(spOwner->_TransformComp);
					bFly=spPhysicTransform->bFly;
					spPhysicTransform->Landing();
					if (spOwner->GetID() == OBJECT_ID::EID::EPLAYER)
					{
						auto _Player =std::dynamic_pointer_cast<Player>(spOwner);
						_Player->bWallJump = false;
					}
			
			    }
				spOwner->MapHit(std::move(_HitInfo));
			}
		}


	}

	// 아래방향 점프가 가능한 타일
	auto& _CollisionDownJumpTileVec = GetCollisionTileContainerRef(true);

	for (auto& _spCollision : _CollisionCompVec)
	{
		bool bCollision = false;
		bool bLand = false;
		bool bFly = false;
		if (!_spCollision->bCollision)continue;
		if (!(_CollisionTagSet.contains(_spCollision->_Tag)))continue;
		if (_spCollision->bDownJump)continue;

		for (auto& _CollisionTile : _CollisionDownJumpTileVec)
		{
			auto WorldRectPt = _spCollision->GetWorldRectPt();

			auto opDir = math::Collision::RectAndRect({ WorldRectPt, _CollisionTile }, false);

			if (opDir)
			{
				bCollision = true;

				vec3 TilePushDir = *opDir; 
				auto spOwner = _spCollision->_Owner.lock();

				if (std::abs(TilePushDir.x) < std::abs(TilePushDir.y))
					TilePushDir.y = 0;
				else		   
					TilePushDir.x = 0;

				// 대상의 바닥 콜리전과 타일 상단의 거리로 판단
				const bool Check = (std::abs(_CollisionTile[0].y - WorldRectPt[3].y) < LandCheckDistance);

				if (spOwner->_TransformComp->bMapSlide && TilePushDir.y < 0.0f && Check)
				{
					spOwner->_TransformComp->Position += TilePushDir;
				}

				math::Collision::HitInfo _HitInfo{};
				_HitInfo.PosDistance = D3DXVec3Length(&(*opDir));
				D3DXVec3Normalize(&(*opDir), &(*opDir));
				_HitInfo.PosDir = *opDir;
				D3DXVec3Normalize(&TilePushDir, &TilePushDir);
				_HitInfo.Normal = TilePushDir;
				_HitInfo.Position = math::GetCenter(_CollisionTile);
				_HitInfo._Target = { };
				_HitInfo._ID = OBJECT_ID::EDOWNJUMPTILE;
				_HitInfo._TAG == OBJECT_TAG::ETERRAIN;
				//밀어낸 이후에 위에 존재한다면 땅에닿았었다는 처리
				if (std::abs(WorldRectPt[2].y - _CollisionTile[0].y) < LandCheckDistance)
				{
			

					bLand = true;
					auto spPhysicTransform = std::dynamic_pointer_cast<PhysicTransformComponent>
						(spOwner->_TransformComp);
					if (!spPhysicTransform)continue;
					bFly = spPhysicTransform->bFly;
					if (spOwner->GetID() == OBJECT_ID::EID::EPLAYER)
					{
						auto _Player = std::dynamic_pointer_cast<Player>(spOwner);
						if (!_Player->bWallJump)
						{
							spPhysicTransform->DownLanding();
						}
					}
				
				}
				spOwner->MapHit(std::move(_HitInfo));
			}
		}
		if (bLand == false)
		{
			auto spOwner = _spCollision->_Owner.lock();
			auto spPhysicTransform = std::dynamic_pointer_cast<PhysicTransformComponent>(spOwner->_TransformComp);
			spPhysicTransform->bCurCollisionTile = false;
			// TODO::
			spPhysicTransform->bDownLand = false;
		}

	}
}

void CollisionTileManager::LateUpdate()&
{
}

void CollisionTileManager::LoadCollisionTile(const std::wstring & FilePath) &
{
	Clear();

	std::wifstream file_output(FilePath);

	std::apply([&file_output](auto&... _CollisionTileContainers) {
		auto FileLoad = [&file_output](_CollisionTileContainerType& _CollisionTileContainer) {

			size_t _InfoSize{};
			file_output >> _InfoSize;

			for (size_t i = 0; i < _InfoSize; ++i)
			{
				_CollisionTileContainerType::value_type _CollisionTilePoints;

				for (auto& _Points : _CollisionTilePoints)
				{
					file_output >> _Points;
				}

				_CollisionTileContainer.push_back(std::move(_CollisionTilePoints));
			}
		};

		(FileLoad(_CollisionTileContainers), ...);
	}, _CollisionTilePointsMap[CurrentStateKey]);
}

void CollisionTileManager::SaveCollisionTile(const std::wstring & FilePath) &
{
	std::wofstream file_Input(FilePath);
	// 현재 맵 모드 의 모든 레이어 의 사이즈가 곧 타일의 개수
	std::apply([&file_Input](const auto&... _CollisionTileContainers) {
		auto FileSave = [&file_Input](const _CollisionTileContainerType& _CollisionTileContainer) {

			size_t _InfoSize = _CollisionTileContainer.size();

			file_Input << _InfoSize << std::endl;

			for (const auto& CurrentCollisionTilePoints : _CollisionTileContainer)
			{
				for (const auto& Points : CurrentCollisionTilePoints)
				{
					file_Input << Points;
				}
			}
		};

		(FileSave(_CollisionTileContainers), ...);

	}, _CollisionTilePointsMap[CurrentStateKey]);
}

void CollisionTileManager::Clear() &
{
	_CollisionTilePointsMap.clear();
}



typename CollisionTileManager::_CollisionTileContainerType & CollisionTileManager::GetCollisionTileContainerRef(const bool bCanGoDown) &
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return (!bCanGoDown) ? _CollisionTilePointsMap[CurrentStateKey].first : _CollisionTilePointsMap[CurrentStateKey].second;
}




