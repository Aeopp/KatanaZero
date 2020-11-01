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
	return os << _Info._Idx << endl << _Info.WorldLocation << endl << (int32_t&)_Info._Mode;
}

static std::wistream& operator>>(std::wistream& is, NodeInfo& _Info)
{
	return is >> _Info._Idx >> _Info.WorldLocation >> (int32_t&)_Info._Mode;
}
// ���������� �ε��ҽ� �浹 Ÿ�� ������ Ŭ�����ϰ� �ش� ���������� �°� ������Ʈ ���ּ���.
class AStarManager
{
public :
	void DebugRender();
	void PushNodeFromWorldLocation(const vec3& WorldLocation,bool bDoor,bool bStair, bool bStairEnd);
	void EraseNodeFromWorldLocation(const vec3& WorldLocation);
	void Clear() & noexcept;
	void Load(const std::wstring& FilePath);
	void Save(const std::wstring& FilePath);
private:
	const int32_t DoorCheckMin = 30;
	// ���� ��ǥ�κ��� ����� �ο�� �÷��� ��ȯ
	D3DXCOLOR GetColorFromNodeOpt(NodeInfo::EMODE _Mode);
	NodeIdx ConvertNodeIdx(const vec3& WorldLocation);
	vec3 ConvertWorldLocation(const NodeIdx& _NodeIdx);
	// ������� ����ؼ� �׷����� ����.
	void MakeGraph();
	// 8������ �˻��� ��������Ͽ� �߰�.
	void AdjNodePush(NodeInfo& _Info);
	std::shared_ptr<NodeInfo> FindNodeFromColRow(NodeIdx _Idx);
	std::array<vec2, 4ul> ConvertWorldRtPt(const vec3& NodeLocation);
					   // Col					// Row 
	std::unordered_map<int32_t, std::unordered_map<int32_t, std::shared_ptr<NodeInfo>>> 
	_NodeMap;
				// Door Idx Info ����
	std::vector<NodeIdx> _DoorIdxVec;
};


