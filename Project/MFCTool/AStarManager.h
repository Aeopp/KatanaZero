#pragma once
#include "TupleGlobalHelper.h"

struct NodeIdx
{
	int32_t Col = 0;
	int32_t Row = 0;
	friend std::wostream& operator<<(std::wostream& os, NodeIdx& _Idx);
	friend std::wistream& operator>>(std::wistream& is, NodeIdx& _Idx);
};

static std::wistream& operator>>(std::wistream& is , NodeIdx& _Idx)
{
	return is >> _Idx.Col >>  _Idx.Row;
}

static std::wostream& operator<<(std::wostream& os, NodeIdx& _Idx)
{
	return os << _Idx.Col << std::endl << _Idx.Row;
}

struct NodeInfo
{
	enum class EMODE : int32_t
	{
		None,
		Door,
		Stair,
		DoorStair,
	};

	NodeIdx _Idx{0,0};
	vec3 WorldLocation{ 0.f,0.f,0.f };
	EMODE _Mode = { EMODE::None };

	std::vector<std::reference_wrapper<NodeInfo>> AdjInfos;

	friend std::wostream& operator<<(std::wostream& os, NodeInfo& _Info);
	friend std::wistream& operator>>(std::wistream& is, NodeInfo& _Info);
};

static std::wostream& operator<<(std::wostream& os, NodeInfo& _Info)
{
	return os << _Info._Idx << endl << _Info.WorldLocation << endl << (int32_t&)_Info._Mode;
}

static std::wistream& operator>>(std::wistream& is, NodeInfo& _Info)
{
	return is >> _Info._Idx >> _Info.WorldLocation >> (int32_t&)_Info._Mode;
}
// 스테이지를 로딩할시 충돌 타일 정보를 클리어하고 해당 스테이지에 맞게 업데이트 해주세요.
class AStarManager
{
public :
	void DebugRender();
	void PushNodeFromWorldLocation(const vec3& WorldLocation,bool bDoor,bool bStair);
	void EraseNodeFromWorldLocation(const vec3& WorldLocation);
	void Clear() & noexcept;
	void Load(const std::wstring& FilePath);
	void Save(const std::wstring& FilePath);
private:
	// 월드 좌표로부터 노드의 로우와 컬럼을 반환
	D3DXCOLOR GetColorFromNodeOpt(NodeInfo::EMODE _Mode);
	NodeIdx ConvertNodeIdx(const vec3& WorldLocation);
	vec3 ConvertWorldLocation(const NodeIdx& _NodeIdx);
	// 인접노드 계산해서 그래프를 구축.
	void MakeGraph();
	// 8방향을 검색해 인접노드목록에 추가.
	void AdjNodePush(NodeInfo& _Info);
	std::shared_ptr<NodeInfo> FindNodeFromColRow(NodeIdx _Idx);
	std::array<vec2, 4ul> ConvertWorldRtPt(const vec3& NodeLocation);
					   // Col					// Row 
	std::unordered_map<int32_t, std::unordered_map<int32_t, std::shared_ptr<NodeInfo>>> 
	_NodeMap;
				// Door Idx Info 저장
	std::vector<NodeIdx> _DoorIdxVec;
};


