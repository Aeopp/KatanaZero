#pragma once
#include "singleton_interface.h"
#include <set>
#include "CollisionComponent.h"

// 스테이지를 로딩할시 충돌 타일 정보를 클리어하고 해당 스테이지에 맞게 업데이트 해주세요.
class CollisionTileManager : public singleton_interface<CollisionTileManager>
{
public:
	using _CollisionTileType = std::array<vec3, 4ul>;
	using _CollisionTileContainerType = std::vector<typename CollisionTileManager::_CollisionTileType>;
	using _StageKeyType = std::wstring; 
	using _CollisionTileMapType = std::map<_StageKeyType, std::pair </*아래로 내려갈수 없는 타일*/_CollisionTileContainerType ,/*아래로 내려갈수 있는 타일*/_CollisionTileContainerType > >;
private:
	// 충돌 검출 , 디버그 드로잉 할시 연산 하는게 아님.
	// 미리 포인트 4개를 구해놓는다.
	//           Key StateKey        Value Tile Points
	_CollisionTileMapType _CollisionTilePointsMap;
	void Collision()&;
	
	
public:
	void Initialize() & noexcept;

	std::wstring CurrentStateKey;
	void Push(const vec3& Position,const bool bCanGoDown)&;
	void Erase(const vec3& TargetPosition,const bool bCanGoDown)&;
	void DebugRender()&;
	void Update()&;
	void LateUpdate()&;
	void LoadCollisionTile(const std::wstring& FilePath)&;
	void SaveCollisionTile(const std::wstring& FilePath) &; 
	void Clear()&;

	// 해당 집합에 존재하지 않으면 충돌검사 수행 X
	const float LandCheckDistance = 10.f;
	const float TileCollisionCheckDistanceMinSquare = 20000.f;
	_CollisionTileContainerType& GetCollisionTileContainerRef(const bool bCanGoDown)&;
private:
	std::set<typename CollisionComponent::ETag> _CollisionTagSet
	{
		CollisionComponent::ETag::EPlayer,
		CollisionComponent::ETag::EEnemy,
		CollisionComponent::ETag::ECollisionEffect,
		CollisionComponent::ETag::EItem,
		CollisionComponent::ETag::ESmoke_Cloud,
	};
};

