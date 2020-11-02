#pragma once
#include "singleton_interface.h"
#include <queue>
#include <set>

struct NodeIdx
{
	int32_t Col = 0;
	int32_t Row = 0;

	friend std::wostream& operator<<(std::wostream& os, NodeIdx& _Idx);
	friend std::wistream& operator>>(std::wistream& is, NodeIdx& _Idx);
};

namespace std
{
	template<>
	struct hash<NodeIdx>
	{
		size_t operator() (const NodeIdx& Rhs)const
		{
			hash<int32_t> hash_func;
			
			return hash_func(Rhs.Col) ^ hash_func(Rhs.Row);
		}
	};
}

static bool operator==(const NodeIdx& Lhs, const NodeIdx& Rhs)
{
	return Rhs.Col == Lhs.Col && Lhs.Row == Rhs.Row;
};

static bool operator!=(const NodeIdx& Lhs, const NodeIdx& Rhs)
{
	return !(Lhs == Rhs);
};

struct Cost
{
	float F = 0;
	NodeIdx _Idx;
};

struct Info
{
	float G = 0, H = 0;
	NodeIdx Parent; 
};

static auto Comp = [](const Cost& lhs, const Cost& rhs) ->bool {
	return  lhs.F > rhs.F;
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
		StairEnd,
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
	return os << _Info._Idx << std::endl << _Info.WorldLocation << std::endl << (int32_t&)_Info._Mode;
}

static std::wistream& operator>>(std::wistream& is, NodeInfo& _Info)
{
	return is >> _Info._Idx >> _Info.WorldLocation >> (int32_t&)_Info._Mode;
}
// 스테이지를 로딩할시 충돌 타일 정보를 클리어하고 해당 스테이지에 맞게 업데이트 해주세요.
class AStarManager : public singleton_interface<AStarManager>
{
public :
	void DebugRender();
	void PushNodeFromWorldLocation(const vec3& WorldLocation,bool bDoor,bool bStair, bool bStairEnd);
	void EraseNodeFromWorldLocation(const vec3& WorldLocation);
	void Clear() & noexcept;
	void Load(const std::wstring& FilePath);
	void Save(const std::wstring& FilePath);

	std::vector<vec3> PathFind(const vec3& vStart, const vec3& vGoal);

	NodeIdx ConvertNodeIdx(const vec3& WorldLocation);
private:
	const int32_t DoorCheckMin = 30;
	// 월드 좌표로부터 노드의 로우와 컬럼을 반환
	D3DXCOLOR GetColorFromNodeOpt(NodeInfo::EMODE _Mode);

	vec3 ConvertWorldLocation(const NodeIdx& _NodeIdx);
	// 인접노드 계산해서 그래프를 구축.
	void MakeGraph();
	// 8방향을 검색해 인접노드목록에 추가.
	void AdjNodePush(NodeInfo& _Info);
	std::shared_ptr<NodeInfo> FindNodeFromColRow(NodeIdx _Idx);
	std::array<vec2, 4ul> ConvertWorldRtPt(const vec3& NodeLocation);
					   // Col					// Row 
	std::unordered_map<int32_t, std::unordered_map<int32_t, std::shared_ptr<NodeInfo>>> 	_NodeMap;
				// Door Idx Info 저장
	std::vector<NodeIdx> _DoorIdxVec;
private:
	std::vector<vec2> RecentPath;

	float Heuristics(const vec3& Lhs, const vec3& Rhs);

	         // Col            // Row
	std::unordered_map<NodeIdx,Info> _Open;
			// Col            // Row
	std::unordered_map<NodeIdx,NodeIdx>_Close;
};


