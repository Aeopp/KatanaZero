#include "stdafx.h"
#include "AStarManager.h"
#include "math.h"
#include "GraphicDevice.h"
#include <fstream>
#include <ostream>
#include <istream>

using namespace std;

void AStarManager::DebugRender()
{
	if (!global::bDebug)return;
	static constexpr float DebugLineWidth = 2.f;
	std::pair<float, float > CameraPos{ 0.f,0.f };
	float JoomScale = 1.f;
	matrix MJoom;

#ifdef _AFX
	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (!pMain)return;
	CMFCToolView* pView = dynamic_cast<CMFCToolView*>(pMain->_MainSplitter.GetPane(0, 1));
	if (!pView)return;

	CameraPos.first = pView->GetScrollPos(0);
	CameraPos.second = pView->GetScrollPos(1);

	JoomScale = pView->JoomScale;
#endif
	CameraPos.first = global::CameraPos.x;
	CameraPos.second = global::CameraPos.y;
	JoomScale = global::JoomScale;

	GraphicDevice::instance().GetSprite()->End();
	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);

	int32_t RenderCount = 0;
	int32_t TileCount = 0;
	MJoom = math::GetCameraJoomMatrix(JoomScale,
		vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

	for (auto& Col_RowMap : _NodeMap)
	{
		for (auto& Row_spInfo : Col_RowMap.second)
		{
			++ TileCount; 

			auto& _spInfo = Row_spInfo.second;

			auto _RtPt2D = ConvertWorldRtPt(_spInfo->WorldLocation);
			std::array<vec3, 4ul> _RtPt3D;
			bool bRenderable = false;
			for (size_t i = 0; i < _RtPt3D.size(); ++i)
			{
				_RtPt2D[i].x -= CameraPos.first;
				_RtPt2D[i].y -= CameraPos.second;
				D3DXVec2TransformCoord(&_RtPt2D[i], &_RtPt2D[i], &MJoom);
				_RtPt3D[i] = { _RtPt2D[i].x,_RtPt2D[i].y,0.f };
				bRenderable |=math::IsPointInnerRect(global::GetScreenRect(), _RtPt3D[i]);
			}

			if (bRenderable)
			{
				++RenderCount;
				std::array<vec2, 5ul> _RtPt2D5Pt;
				for (size_t i = 0; i < _RtPt2D.size(); ++i)
				{
					_RtPt2D5Pt[i] = std::move(_RtPt2D[i]);
				}
				_RtPt2D5Pt.back() = _RtPt2D5Pt.front();

				GraphicDevice::instance().GetLine()->Draw(_RtPt2D5Pt.data(),
					_RtPt2D5Pt.size(),GetColorFromNodeOpt(_spInfo->_Mode));
			}
		}
	}
	if (!RecentPath.empty())
	{
		auto RenderRecentPath = RecentPath;
		bool bRenderable;
		for (size_t i = 0; i < RenderRecentPath.size(); ++i)
		{
			RenderRecentPath[i].x -= CameraPos.first;
			RenderRecentPath[i].y -= CameraPos.second;
			D3DXVec2TransformCoord(&RenderRecentPath[i], &RenderRecentPath[i], &MJoom);
		}
		GraphicDevice::instance().GetLine()->Draw(RenderRecentPath.data(), RenderRecentPath.size(), D3DCOLOR_ARGB(255, 123, 213, 55));
	}
	

	std::wstring DebugTileStr = L"RenderDebugAStarNode : " + std::to_wstring(RenderCount);
	RECT rectRender{ 0,800,500,825};
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	DebugTileStr = L"CullingDebugAStarNode : " + std::to_wstring(TileCount - RenderCount);
	rectRender = { 0,850,500,875};
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void AStarManager::PushNodeFromWorldLocation(const vec3& WorldLocation, bool bDoor, bool bStair)
{
	NodeInfo _Info;
	_Info._Idx = ConvertNodeIdx(WorldLocation);
	_Info.WorldLocation = ConvertWorldLocation(_Info._Idx);

	if (bDoor && bStair)_Info._Mode = NodeInfo::EMODE::DoorStair;
	else if (bDoor)	_Info._Mode = NodeInfo::EMODE::Door;
	else if (bStair)_Info._Mode = NodeInfo::EMODE::Stair;
	else _Info._Mode = NodeInfo::EMODE::None;

	/*if (bDoor)
	{
		_DoorIdxVec.emplace_back(_Info._Idx);
	}*/
	_NodeMap[_Info._Idx.Col][_Info._Idx.Row] = std::make_shared<NodeInfo>
		(
			std::move(_Info)
		);
};

void AStarManager::EraseNodeFromWorldLocation(const vec3& WorldLocation)
{
	NodeIdx _Idx = ConvertNodeIdx(WorldLocation);
	if (_NodeMap.contains(_Idx.Col))
	{
		_NodeMap[_Idx.Col].erase(_Idx.Row);
	}
};

void AStarManager::Clear() & noexcept
{
	_NodeMap.clear();
}

void AStarManager::Load(const std::wstring& FilePath)
{
	Clear();

	std::wifstream _if(FilePath);

	size_t Count = 0;

	_if >> Count;

	for (size_t i = 0; i < Count; ++i)
	{
		auto _spInfo = std::make_shared<NodeInfo>();
		_if >> *_spInfo;
		_NodeMap[_spInfo->_Idx.Col][_spInfo->_Idx.Row] = (_spInfo);

		if (_spInfo->_Mode == NodeInfo::EMODE::Door ||
			_spInfo->_Mode == NodeInfo::EMODE::DoorStair)
		{
			_DoorIdxVec.emplace_back(NodeIdx{ _spInfo ->_Idx});
		}
	};

	MakeGraph();
}

void AStarManager::Save(const std::wstring& FilePath)
{
	std::wofstream _of(FilePath);
	
	size_t Count = std::accumulate(std::begin(_NodeMap), std::end(_NodeMap), 0ul,
		[](size_t Count, auto& Col_Map)
	{
		return Count + Col_Map.second.size();
	});

	_of << Count << std::endl;

	for (auto& ColMap : _NodeMap)
	{
		for (auto& RowMap : ColMap.second)
		{
			auto _spInfo = RowMap.second;
			_of << *_spInfo << std::endl;
		}
	}
}

D3DXCOLOR AStarManager::GetColorFromNodeOpt(NodeInfo::EMODE _Mode)
{
	switch (_Mode)
	{
	case NodeInfo::EMODE::None:
		return D3DCOLOR_ARGB(255, 255, 0, 229);
		break;
	case NodeInfo::EMODE::Door:	
		return D3DCOLOR_ARGB(255, 0, 85, 255);
		break;
	case NodeInfo::EMODE::Stair:
		return D3DCOLOR_ARGB(255, 0, 246, 255);
		break;
	case NodeInfo::EMODE::DoorStair:
		return D3DCOLOR_ARGB(255, 255, 255, 255);
		break;
	default:
		break;
	}

	return D3DCOLOR_ARGB(255, 255, 0, 229);
}

NodeIdx AStarManager::ConvertNodeIdx(const vec3& WorldLocation)
{
	return {  int32_t ( WorldLocation.x / global::TileSize.first ), 
			int32_t ( WorldLocation.y / global::TileSize.second) };
};

vec3 AStarManager::ConvertWorldLocation(const NodeIdx& _NodeIdx)
{
	return
	{
		_NodeIdx.Col * global::TileSize.first + global::TileSize.first/2.f,
		_NodeIdx.Row * global::TileSize.second + global::TileSize.second / 2.f,
		0.f,
	};
}
void AStarManager::MakeGraph()
{
	for (auto& ColMap : _NodeMap)
	{
		for (auto& RowMap : ColMap.second)
		{
			auto _spInfo = RowMap.second;

			AdjNodePush(*_spInfo);
		}
	}
}
void AStarManager::AdjNodePush(NodeInfo& _Info)
{
	// 상단
	auto spInfo=FindNodeFromColRow({ _Info._Idx.Col - 1,_Info._Idx.Row - 1 });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);
	spInfo = FindNodeFromColRow({ _Info._Idx.Col,_Info._Idx.Row - 1 });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);
	spInfo=FindNodeFromColRow({ _Info._Idx.Col + 1,_Info._Idx.Row - 1 });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);

	// 중앙
	spInfo = FindNodeFromColRow({ _Info._Idx.Col - 1,_Info._Idx.Row });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);
	spInfo = FindNodeFromColRow({ _Info._Idx.Col + 1,_Info._Idx.Row });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);

	// 하단
	spInfo = FindNodeFromColRow({ _Info._Idx.Col-1,_Info._Idx.Row+1 });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);
	spInfo = FindNodeFromColRow({ _Info._Idx.Col,_Info._Idx.Row +1});
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo);
	spInfo = FindNodeFromColRow({ _Info._Idx.Col +1,_Info._Idx.Row+1 });
	if (spInfo)
		_Info.AdjInfos.push_back(*spInfo); 

	 // 문이라면 같은 건물의 문인지 검사한 이후에 노드로 연결
	if (_Info._Mode == NodeInfo::EMODE::Door || _Info._Mode == NodeInfo::EMODE::DoorStair)
	{
		for (auto& Idx : _DoorIdxVec)
		{
			if (Idx.Col == _Info._Idx.Col)
			{
				spInfo =FindNodeFromColRow(Idx);
				if (spInfo)
					_Info.AdjInfos.push_back(*spInfo);
			}
		}
	}
}

std::array<vec2, 4ul> AStarManager::ConvertWorldRtPt(const vec3& NodeLocation)
{
	const float halfX = (float)global::TileSize.first / 2.f ;
	const float halfY = (float)global::TileSize.second / 2.f;

	std::array<vec2, 4ul> _ReturnVal;

	// LeftTop 
	_ReturnVal[0] = { NodeLocation.x - halfX, NodeLocation.y - halfY };
	// RightTop 
	_ReturnVal[1] = { NodeLocation.x  + halfX, NodeLocation.y - halfY };
	// RightBottom 
	_ReturnVal[2] = { NodeLocation.x  + halfX, NodeLocation.y + halfY };
	// LeftBottom  
	_ReturnVal[3] = { NodeLocation.x - halfX, NodeLocation.y +  halfY };

	return _ReturnVal;
}
std::vector<vec3> AStarManager::PathFind(const vec3& vStart, const vec3& vGoal)
{
	NodeIdx StartIdx= ConvertNodeIdx(vStart);
	NodeIdx GoalIdx = ConvertNodeIdx(vGoal);
	std::shared_ptr<NodeInfo> spStartInfo, spGoalInfo;

	std::priority_queue<Cost, std::vector<Cost>, decltype(Comp)> _MinF{ Comp };

	_Open.clear();
	_Close.clear();

	while (true)
	{
		spStartInfo =FindNodeFromColRow(StartIdx);
		if (spStartInfo)
		{
			break;
		}
		else
		{
			++StartIdx.Row;
			continue; 
		}
	}

	while (true)
	{
		spGoalInfo = FindNodeFromColRow(GoalIdx);
		if (spGoalInfo)
		{
			break; 
		}
		else
		{
			++GoalIdx.Row;
			continue;
		}
	}

	const vec3 StartLocation = spStartInfo->WorldLocation;
	const vec3 GoalLocation = spGoalInfo->WorldLocation;

	Info _StartInfo;
	_StartInfo.Parent = spStartInfo->_Idx;
	_StartInfo.G = 0;
	_StartInfo.H = Heuristics(GoalLocation, StartLocation);

	Cost _StartCost;
	_StartCost.F = _StartInfo.G + _StartInfo.H;
	_StartCost._Idx = _StartInfo.Parent;

	_Open[_StartInfo.Parent]= _StartInfo;

	_MinF.push(_StartCost);

	while (!_MinF.empty())
	{
		Cost _CurMinF = _MinF.top();
		_MinF.pop();

		if (_CurMinF._Idx == GoalIdx)
		{
			NodeIdx CurIdx = GoalIdx;
			std::vector<vec3> Paths;

			_Close[GoalIdx] = _Open[GoalIdx].Parent;

			while (CurIdx != StartIdx)
			{
				Paths.emplace_back(
				_NodeMap[CurIdx.Col][CurIdx.Row]->WorldLocation);

				CurIdx = _Close[CurIdx];
			}

			if (!global::bDebug)
			{
				return Paths;
			}
			else
			{
				RecentPath.clear();
				for (auto& Path : Paths)
				{
					RecentPath.emplace_back(vec2{ Path.x,Path.y });
				}
				return Paths;
			}
		}

		if (_Close.contains(_CurMinF._Idx)) continue;

		const NodeIdx CurSltIdx = _CurMinF._Idx;
		const vec3 CurSltPos = _NodeMap[CurSltIdx.Col][CurSltIdx.Row]->WorldLocation;
		const NodeIdx CurSltParent = _Open[CurSltIdx].Parent;
		const int32_t CurSltG = _Open[CurSltIdx].G;

		for (auto& AdjInfo : _NodeMap[CurSltIdx.Col][CurSltIdx.Row]->AdjInfos)
		{
			Cost _AdjCost;

			const NodeIdx CurAdjIdx = AdjInfo.get()._Idx;
			const vec3 CurAdjLocation = _NodeMap[CurAdjIdx.Col][CurAdjIdx.Row]->WorldLocation;

			// 이것이 닫힌 목록에 있다면 무시합니다.
			if (_Close.contains(CurAdjIdx))continue;

			if ( !_Open.contains(CurAdjIdx) ) 
			{
				Info _AdjInfo;
				_AdjInfo.Parent = CurSltIdx;
				_AdjInfo.H = Heuristics(GoalLocation, CurAdjLocation);
				_AdjInfo.G = Heuristics(CurSltPos, CurAdjLocation);

				_AdjCost.F = _AdjInfo.H + _AdjInfo.G;
				_AdjCost._Idx = CurAdjIdx;

				_Open[CurAdjIdx]= std::move(_AdjInfo);
				_MinF.push(std::move(_AdjCost));
			}
			else
			{
				int32_t GFromParent = CurSltG + Heuristics(CurSltPos, CurAdjLocation);
				Info& _InfoAmend = _Open[CurAdjIdx];

				if (GFromParent < _InfoAmend.G)
				{
					_InfoAmend.G = GFromParent;
					_InfoAmend.Parent = CurSltIdx;
					_AdjCost.F = _InfoAmend.G + _InfoAmend.H;
					_AdjCost._Idx = CurAdjIdx;
				}
			}
		}
		_Open.erase(CurSltIdx);
		_Close[CurSltIdx] = CurSltParent;
	}



	return std::vector<vec3>();
}
float AStarManager::Heuristics(const vec3& Lhs, const vec3& Rhs)
{
	/*return (std::max)
		(std::abs(Lhs.x - Rhs.x),
		std::abs(Lhs.y - Rhs.y));*/

	vec3 Dis = Lhs - Rhs;
	return D3DXVec3LengthSq(&Dis);
};


std::shared_ptr<NodeInfo> AStarManager::FindNodeFromColRow(NodeIdx _Idx)
{
	auto ColFind = _NodeMap.find(_Idx.Col);
	if (ColFind != end(_NodeMap))
	{
		auto RowFind = ColFind->second.find(_Idx.Row);
		if (RowFind != end(ColFind->second))
		{
			return RowFind->second;
		}
	}

	return {};
}