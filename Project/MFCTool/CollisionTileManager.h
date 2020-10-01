#pragma once

// ���������� �ε��ҽ� �浹 Ÿ�� ������ Ŭ�����ϰ� �ش� ���������� �°� ������Ʈ ���ּ���.
class CollisionTileManager
{
private:
	// �浹 ���� , ����� ����� �ҽ� ���� �ϴ°� �ƴ�.
	// �̸� ����Ʈ 4���� ���س��´�.
	std::vector<std::array<vec3, 4ul>> _CollisionTilePointsVec;
public:
	void Push(const vec3& Position)&;
	void Erase(const vec3& TargetPosition)&;
	void DebugRender()const&;
	void LoadCollisionTile(const std::wstring& FilePath)&;
	void SaveCollisionTile(const std::wstring& FilePath) &; 
	void Clear()&;
};

