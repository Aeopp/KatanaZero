#pragma once

// 스테이지를 로딩할시 충돌 타일 정보를 클리어하고 해당 스테이지에 맞게 업데이트 해주세요.
class CollisionTileManager
{
private:
	// 충돌 검출 , 디버그 드로잉 할시 연산 하는게 아님.
	// 미리 포인트 4개를 구해놓는다.
	std::vector<std::array<vec3, 4ul>> _CollisionTilePointsVec;
public:
	void Push(const vec3& Position)&;
	void Erase(const vec3& TargetPosition)&;
	void DebugRender()const&;
	void LoadCollisionTile(const std::wstring& FilePath)&;
	void SaveCollisionTile(const std::wstring& FilePath) &; 
	void Clear()&;
};

