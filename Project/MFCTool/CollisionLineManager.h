#pragma once

class CollisionLineManager
{
private:
	//                 start ,end point
	std::vector<std::pair<vec3,vec3>> _CollisionLineVec;
public :
	void Push(std::pair<vec3, vec3> Line)&;
	void Erase(const vec3& TargetPosition)&;
	void DebugRender() const &;
	void LoadCollisionLine(const std::wstring& FilePath)&;
	void SaveCollisionLine(const std::wstring& FilePath)&;
	void Clear()&;
};