#include "stdafx.h"
#include "CollisionTileManager.h"
#include "math.h"
#include "MFCToolView.h"
#include "MainFrm.h"
#include "Graphic_Device.h"
#include <fstream>
#include <ostream>
#include <istream>

void CollisionTileManager::Push(const vec3 & Position) &
{
	float SizeXHalf = global::TileSize.first / 2;
	float SizeYHalf = global::TileSize.second / 2;

	float CollisionTileX = std::floor(Position.x / global::TileSize.first) 
		*  global::TileSize.first + SizeXHalf;

	float CollisionTileY = std::floor(Position.y / global::TileSize.second) * 
		global::TileSize.second + SizeYHalf;

	decltype(_CollisionTilePointsVec)::value_type _Points;

	_Points[0] = { CollisionTileX - SizeXHalf , CollisionTileY - SizeYHalf, 0.f };
	_Points[1] = { CollisionTileX + SizeXHalf , CollisionTileY - SizeYHalf, 0.f };
	_Points[2] = { CollisionTileX + SizeXHalf , CollisionTileY + SizeYHalf, 0.f };
	_Points[3] = { CollisionTileX - SizeXHalf , CollisionTileY + SizeYHalf, 0.f };

	const vec3 InsertPoint = { CollisionTileX,CollisionTileY,0.f };

	auto IsContain = [InsertPoint](const auto& TilePoints)
	{
		const float x=  TilePoints[0].x + (TilePoints[2].x - TilePoints[0].x) / 2.f;
		const float y = TilePoints[0].y + (TilePoints[2].y - TilePoints[0].y) / 2.f;

		const vec3 CurrentTileCenter{ x,y,0.f };

		return InsertPoint == CurrentTileCenter;
	};

	auto IsFindIter = std::find_if(std::begin(_CollisionTilePointsVec), std::end(_CollisionTilePointsVec),
		std::move(IsContain));

	bool IsInsertable = (IsFindIter == std::end(_CollisionTilePointsVec));

	if (IsInsertable)
		_CollisionTilePointsVec.push_back(std::move(_Points));
}

void CollisionTileManager::Erase(const vec3 & TargetPosition) &
{
	 auto IsFindIter = std::find_if(std::begin
		(_CollisionTilePointsVec),std::end(_CollisionTilePointsVec),
		[TargetPosition](const auto& CurrentTilePoints) 
		{
			return math::IsPointInnerRect(CurrentTilePoints, TargetPosition);
		});

	if ( IsFindIter !=_CollisionTilePointsVec.end())
		_CollisionTilePointsVec.erase(IsFindIter);
}

void CollisionTileManager::DebugRender() const &
{
	if (!global::bDebug)return;
	constexpr float DebugLineWidth = 2.f;

	std::pair<float, float > CameraPos{ 0.f,0.f }; 

	#ifdef _AFX
		CMainFrame*pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		if (!pMain)return;
		CMFCToolView*pView = dynamic_cast<CMFCToolView*>(pMain->_MainSplitter.GetPane(0, 1));
		if (!pView)return;

		CameraPos.first = pView->GetScrollPos(0);
		CameraPos.second = pView->GetScrollPos(1);
	#endif

	GraphicDevice::instance().GetSprite()->End();
	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);

	uint32_t RenderCount = 0;

	for (const auto& CollisionTilePoints : _CollisionTilePointsVec)
	{
		bool IsRenderable = false;
		std::array < vec2, 4ul > CollisionTilePoints2D;
		// Convert 3D->2D
		for (size_t i = 0; i < 4; ++i)
		{
			CollisionTilePoints2D[i] = { CollisionTilePoints[i].x - CameraPos.first ,
										CollisionTilePoints[i].y - CameraPos.second };

			vec3 Point3D = { CollisionTilePoints2D[i].x, CollisionTilePoints2D[i].y, 0.f };

			IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), Point3D);
		}
		
		if (IsRenderable)
		{
			++RenderCount;

			GraphicDevice::instance().GetLine()->Draw(CollisionTilePoints2D.data(),
				CollisionTilePoints.size(), (D3DCOLOR_ARGB(123, 255, 0, 0)));
		}
	}

	std::wstring DebugTileStr = L"RenderDebugCollisionTile : " + std::to_wstring(RenderCount);
	RECT rectRender{ 0,50,500,75 };
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	DebugTileStr = L"CullingDebugCollisionTile : " + std::to_wstring(_CollisionTilePointsVec.size() - RenderCount);
	rectRender = { 0,75,500,100};
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void CollisionTileManager::LoadCollisionTile(const std::wstring & FilePath) &
{
	std::wifstream file_output(FilePath);
	size_t _InfoSize{};
	file_output >> _InfoSize;

	for (size_t i = 0; i < _InfoSize; ++i)
	{
		decltype(_CollisionTilePointsVec)::value_type _CollisionTilePoints;

		for (auto& _Points : _CollisionTilePoints)
		{
			file_output >> _Points;
		}

		_CollisionTilePointsVec.push_back(std::move(_CollisionTilePoints));
	}
}

void CollisionTileManager::SaveCollisionTile(const std::wstring & FilePath) &
{
	std::wofstream file_Input(FilePath);

	// 현재 맵 모드 의 모든 레이어 의 사이즈가 곧 타일의 개수
	size_t _InfoSize = _CollisionTilePointsVec.size();

	file_Input << _InfoSize << std::endl;

	for (const auto& CollisionTilePoints : _CollisionTilePointsVec)
	{
		for (const auto& Point: CollisionTilePoints)
		{
			file_Input << Point;
		}
	}
}

void CollisionTileManager::Clear() &
{
	_CollisionTilePointsVec.clear();
}




