#pragma once
#include "singleton_interface.h"
#include <set>
#include "CollisionComponent.h"

// ���������� �ε��ҽ� �浹 Ÿ�� ������ Ŭ�����ϰ� �ش� ���������� �°� ������Ʈ ���ּ���.
class CollisionTileManager : public singleton_interface<CollisionTileManager>
{
public:
	using _CollisionTileType = std::array<vec3, 4ul>;
	using _CollisionTileContainerType = std::vector<typename CollisionTileManager::_CollisionTileType>;
	using _StageKeyType = std::wstring; 
	using _CollisionTileMapType = std::map<_StageKeyType, std::pair </*�Ʒ��� �������� ���� Ÿ��*/_CollisionTileContainerType ,/*�Ʒ��� �������� �ִ� Ÿ��*/_CollisionTileContainerType > >;
private:
	// �浹 ���� , ����� ����� �ҽ� ���� �ϴ°� �ƴ�.
	// �̸� ����Ʈ 4���� ���س��´�.
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

	// �ش� ���տ� �������� ������ �浹�˻� ���� X
	const float LandCheckDistance = 10.f;
	const float TileCollisionCheckDistanceMinSquare = 3000.f;
	_CollisionTileContainerType& GetCollisionTileContainerRef(const bool bCanGoDown)&;
private:
	std::set<typename CollisionComponent::ETag> _CollisionTagSet
	{
		CollisionComponent::ETag::EPlayer,
		CollisionComponent::ETag::EEnemy,
		CollisionComponent::ETag::ECollisionEffect,
		CollisionComponent::ETag::EItem,
	};
};

