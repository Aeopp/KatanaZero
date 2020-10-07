#pragma once

// ���������� �ε��ҽ� �浹 Ÿ�� ������ Ŭ�����ϰ� �ش� ���������� �°� ������Ʈ ���ּ���.
class CollisionTileManager
{
private:
	// �浹 ���� , ����� ����� �ҽ� ���� �ϴ°� �ƴ�.
	// �̸� ����Ʈ 4���� ���س��´�.
	//           Key StateKey        Value Tile Points
	std::map<std::wstring,std::vector<std::array<vec3, 4ul>>> _CollisionTilePointsMap;
public:
	std::wstring CurrentStateKey;
	void Push(const vec3& Position)&;
	void Erase(const vec3& TargetPosition)&;
	void DebugRender()&;
	void LoadCollisionTile(const std::wstring& FilePath)&;
	void SaveCollisionTile(const std::wstring& FilePath) &; 
	void Clear()&;
};

