#pragma once
#include "singleton_interface.h"

class CollisionLineManager :  public singleton_interface<CollisionLineManager>
{
public:
	using LineContainerType = std::vector<std::pair</*StartPoint*/vec3, /*EndPoin*/vec3>>;

	using CollisionLineMapType = std::map<std::wstring/*CurrentMapStateKey*/,
		std::pair /*F=NormalLine,S=WallRideLine*/<LineContainerType, LineContainerType> >;
private:								
	CollisionLineMapType _CollisionLineMap;
public:
	std::wstring CurrentStateKey;
	void Push(std::pair<vec3, vec3> Line, const bool bWallride)&;
	void Erase(const vec3& TargetPosition, const bool bWallride)&;
	void DebugRender() &;
	void Update()&;
	void LateUpdate()&;
	void LoadCollisionLine(const std::wstring& FilePath)&;
	void SaveCollisionLine(const std::wstring& FilePath)&;
	void Clear()&;
	// �Ϲ� ���а� ��Ÿ�Ⱑ ������ ������ �����մϴ�.
	LineContainerType& GetLineContainer(const bool bWallRide)&;

;
};

