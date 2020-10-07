#pragma once

class CollisionLineManager
{
private:
	//                 start ,end point
	//           Key StateKey , Value Line Point
	std::map<std::wstring,std::vector<std::pair<vec3,vec3>>>_CollisionLineMap;
public :
	std::wstring CurrentStateKey; 
	void Push(std::pair<vec3, vec3> Line)&;
	void Erase(const vec3& TargetPosition)&;
	void DebugRender() &;
	void LoadCollisionLine(const std::wstring& FilePath)&;
	void SaveCollisionLine(const std::wstring& FilePath)&;
	void Clear()&;
};