#include "stdafx.h"
#include "Terrain.h"
#include "math.h"
#include <cassert>
#include <fstream>
#include <ostream>
#include <istream>
#include <numeric>
#include "ObjectManager.h"

#include "GraphicDevice.h"
#include "Texture_Manager.h"

Terrain::Terrain()
{
	const int32_t TileNumX = global::TileNums.first;
	const int32_t TileNumY = global::TileNums.second;

	const int32_t SizeX = global::TileSize.first;
	const int32_t SizeY = global::TileSize.second;

	_DebugTiles.resize(TileNumX *TileNumY);

	for (uint32_t i = 0; i < _DebugTiles.size(); ++i)
	{
		_DebugTiles[i].z = 0.f;
		_DebugTiles[i].x = global::ClientViewMinLeftTop + ((SizeX) / 2) + ((i % TileNumY) * SizeX);
		_DebugTiles[i].y = global::ClientViewMinLeftTop + ((SizeY) / 2) + ((i / TileNumX) * SizeY);
	}

	CurrentTileTextureStateKey = L"Prison";
}

Terrain::~Terrain()
{
	Release();
}

void Terrain::PickingPushMapObj(const vec3& Position, const int32_t DrawID, const ELayer_Map _LayerMap)
{
	for (auto & DebugTilePosition : _DebugTiles)
	{
		if (IsPicking(DebugTilePosition, Position))
		{
			RenderMapObjInfo _PushRenderMapObjInfo;
			_PushRenderMapObjInfo.Position = DebugTilePosition;
			_PushRenderMapObjInfo.DrawID = DrawID;
			_PushRenderMapObjInfo._LayerMap = _LayerMap;

			// 현재 레이어에 동일한 위치에 같은 타일이 중복 삽입되는 것을 방지합니다.
			auto& _LayerMap_RenderMapObjVec = 
				_TilesMap[CurrentTileTextureStateKey][_LayerMap];

				auto is_find = std::find_if(std::begin(_LayerMap_RenderMapObjVec), std::end(_LayerMap_RenderMapObjVec),
					[DrawID, DebugTilePosition](const auto& _RenderMapObj)
				{
					return /*_RenderMapObj.DrawID == DrawID &&*/
						_RenderMapObj.Position == DebugTilePosition;
				});

				if (is_find != std::end(_LayerMap_RenderMapObjVec))
					return;
			//////////////////////////////////////
			_LayerMap_RenderMapObjVec.emplace_back(std::move(_PushRenderMapObjInfo));

			return;
		}
	}
}

void Terrain::DeleteMapObjAtPointLocation(const vec3 & Position, const ELayer_Map _LayerMap)
{
		// 현재 레이어 타겟 포지션의 모든 타일 삭제
		auto& _RenderMapObjVec = _TilesMap[CurrentTileTextureStateKey][_LayerMap];

		auto erase_target = std::find_if(std::begin(_RenderMapObjVec),
			std::end(_RenderMapObjVec), [this, Position](auto& _RenderMapObj)
		{
			return IsPicking(_RenderMapObj.Position, Position);
		});

		if (erase_target == std::end(_RenderMapObjVec))return;

		*erase_target = _RenderMapObjVec.back();

		_RenderMapObjVec.pop_back();
		_RenderMapObjVec.shrink_to_fit();
}

bool Terrain::IsPicking(const vec3& TilePosition, const vec3& Point)
{
	const float SizeX_half = global::TileSize.first / 2.0f;
	const float SizeY_half = global::TileSize.second / 2.0f;

	std::array<vec3, 4> vRectPoint;

	vRectPoint[0] = { TilePosition.x - SizeX_half, TilePosition.y - SizeY_half, 0.f };
	vRectPoint[1] = { TilePosition.x + SizeX_half, TilePosition.y - SizeY_half, 0.f };
	vRectPoint[2] = { TilePosition.x + SizeX_half, TilePosition.y + SizeY_half, 0.f };
	vRectPoint[3] = { TilePosition.x - SizeX_half, TilePosition.y + SizeY_half, 0.f };

	return math::IsPointInnerRect(vRectPoint, Point);
}

void Terrain::DebugRender()
{
	if (!global::bDebug)return;
	if (!bDebugGridRender)return;

	constexpr float DebugLineWidth = 0.5f;
	constexpr float ScreenCenterX = global::ClientSize.first / 2.f;
	constexpr float ScreenCenterY = global::ClientSize.second / 2.f;

	float JoomScale = global::JoomScale;

	
	vec2 CameraPos{ 0.f,0.f };
	matrix MScale, MTranslation, MWorld;

	const float SizeX_half = global::TileSize.first / 2.f;
	const float SizeY_half = global::TileSize.second / 2.f;
#ifdef _AFX
	assert(pView&&__FUNCTIONW__);
	CameraPos = { (float)pView->GetScrollPos(0) ,(float)pView->GetScrollPos(1) };
	JoomScale = pView->JoomScale;
#endif
	CameraPos.x  = global::CameraPos.x;
	CameraPos.y = global::CameraPos.y;

	matrix MJoom = math::GetCameraJoomMatrix
	(JoomScale, vec3{ global::ClientSize.first,
		global::ClientSize.second,0.f });

	GraphicDevice::instance().GetSprite()->End();

	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);

	uint32_t RenderCount = 0;

	// 로컬 사각형 좌표 구하기
	const std::array<vec3, 5ul> LocalPoints =
		math::GetLocalRect(vec2{ global::TileSize.first, global::TileSize.second });

	for (const auto& DebugTilePos : _DebugTiles)
	{
		std::array<vec2, 5ul> WorldPoints;

		std::transform(std::begin(LocalPoints), std::end(LocalPoints), std::begin(WorldPoints),
			[DebugTilePos, CameraPos, ScreenCenterX, ScreenCenterY, JoomScale, SizeX_half, SizeY_half](const auto& LocalPoint)
		{
			vec3 ScreenPoint = LocalPoint + DebugTilePos;

			ScreenPoint.x -= (CameraPos.x + ScreenCenterX);
			ScreenPoint.y -= (CameraPos.y + ScreenCenterY);
			ScreenPoint.x *= JoomScale;
			ScreenPoint.y *= JoomScale;
			ScreenPoint.x += ScreenCenterX;
			ScreenPoint.y += ScreenCenterY;
			return vec2{ ScreenPoint.x,ScreenPoint.y };
		});
		WorldPoints.back() = WorldPoints.front();

		bool IsRenderable = false;

		// 여기서 for 돌며 컬링
		for (const auto& WorldPoint : WorldPoints)
		{
			IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), vec3{ WorldPoint.x,WorldPoint.y,0.f });
		}

		if (IsRenderable)
		{
			++RenderCount;
			GraphicDevice::instance().GetLine()->Draw(WorldPoints.data(), WorldPoints.size(),
				(D3DCOLOR_ARGB(255, 107, 255, 124)));
		}
	}

	std::wstring DebugTileStr = L"RenderDebugTile : " + std::to_wstring(RenderCount);
	RECT rectRender{ 0,0,500,25 };
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	DebugTileStr = L"CullingDebugTile : " + std::to_wstring(_DebugTiles.size() - RenderCount);
	rectRender = { 0,25,500,50 };
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void Terrain::Render()
{
#ifdef _AFX
	assert(pView&&__FUNCTION__);
#endif // _AFX
	DebugRender();

	const float JoomScale = global::JoomScale;
	vec3 CameraPos{ 0.f ,0.f , 0.f };
	CameraPos = global::CameraPos;

	matrix MScale, MWorld;
	D3DXMatrixScaling(&MScale, 1.f, 1.f, 0.f);

	uint32_t RenderCount = 0;

	uint32_t CurrentStateMapObjCount = 0;
	for (auto& layer_TileVec : _TilesMap[CurrentTileTextureStateKey])
	{
		CurrentStateMapObjCount += layer_TileVec.second.size();

		for (auto& MapObj : layer_TileVec.second)
		{
			matrix MTrans;
			// TODO :: 개선할 여지가 있는 코드 => DrawID 로 MapObj 를 한번더 
			//그룹으로 묶어준다면 호출횟수가 많이 줄어든다
			// 프로파일링 이후 수정 요망
			auto sp_TexInfo = TextureManager::instance().Get_TexInfo(L"Map",
				CurrentTileTextureStateKey, MapObj.DrawID);

			if (!sp_TexInfo)continue;

			D3DXMatrixTranslation(&MTrans,

				MapObj.Position.x - CameraPos .x  +
				sp_TexInfo->ImageInfo.Width / 2 - global::TileSize.first / 2,

				MapObj.Position.y - CameraPos .y  +
				sp_TexInfo->ImageInfo.Height / 2 - global::TileSize.second / 2, 0.f);

			MWorld = MScale* MTrans  * math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });
			// TODO :: 자동으로 컬링 해준다면 해당 코드는 삭제바람
			const auto LocalPoints = math::GetLocalRect(vec2{ (float)sp_TexInfo->ImageInfo.Width,(float)sp_TexInfo->ImageInfo.Height });

			bool IsRenderable = false;

			for (const auto& LocalPoint : LocalPoints)
			{
				vec3 WorldPoint{ 0,0,0 };
				D3DXVec3TransformCoord(&WorldPoint, &LocalPoint, &MWorld);
				IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), WorldPoint);
				if (IsRenderable)break;
			}

			//////////////////
			if (IsRenderable)
			{
				++RenderCount;
				RECT srcRect = { 0,0,static_cast<int32_t>(sp_TexInfo->ImageInfo.Width),static_cast<int32_t>(sp_TexInfo->ImageInfo.Height) };
				vec3 TextureCenter = { sp_TexInfo->ImageInfo.Width / 2.f,sp_TexInfo->ImageInfo.Height / 2.f,0.f };
				GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
				GraphicDevice::instance().GetSprite()->Draw(sp_TexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
					D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}
	}

	if (global::bDebug)
	{
		std::wstring DebugTileStr = L"RenderMapTexture : " + std::to_wstring(RenderCount);
		RECT rectRender{ 0,150,500,175 };
		GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

		DebugTileStr = L"CullingMapTexture : " + std::to_wstring(CurrentStateMapObjCount - RenderCount);
		rectRender = { 0,175,500,200 };
		GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));
	}
}
void Terrain::MiniRender()
{
	// Scale 
	constexpr float MiniRenderViewScale = 2.f;

	const float JoomScale = 1.f;
	vec3 CameraPos{ 0.f ,0.f , 0.f };  
#ifdef _AFX
	assert(pView&&__FUNCTION__);
#endif // _AFX
	matrix MScale, MWorld;
	D3DXMatrixScaling(&MScale, 1.f, 1.f, 0.f);

	uint32_t RenderCount = 0;

	uint32_t CurrentStateMapObjCount = 0;
	for (auto& layer_TileVec : _TilesMap[CurrentTileTextureStateKey])
	{
		CurrentStateMapObjCount += layer_TileVec.second.size();

		for (auto& MapObj : layer_TileVec.second)
		{
			matrix MTrans;
			// TODO :: 개선할 여지가 있는 코드 => DrawID 로 MapObj 를 한번더 
			//그룹으로 묶어준다면 호출횟수가 많이 줄어든다
			// 프로파일링 이후 수정 요망
			auto sp_TexInfo = TextureManager::instance().Get_TexInfo(L"Map",
				CurrentTileTextureStateKey, MapObj.DrawID);

			if (!sp_TexInfo)continue;

			D3DXMatrixTranslation(&MTrans,

				MapObj.Position.x - CameraPos.x +
				sp_TexInfo->ImageInfo.Width / 2 - global::TileSize.first / 2,

				MapObj.Position.y - CameraPos.y+
				sp_TexInfo->ImageInfo.Height / 2 - global::TileSize.second / 2, 0.f);

			MWorld = MScale* MTrans  * math::GetCameraJoomMatrix(JoomScale *MiniRenderViewScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });
			// TODO :: 자동으로 컬링 해준다면 해당 코드는 삭제바람
			const auto LocalPoints = math::GetLocalRect(vec2{ (float)sp_TexInfo->ImageInfo.Width,(float)sp_TexInfo->ImageInfo.Height });

			bool IsRenderable = false;

			for (const auto& LocalPoint : LocalPoints)
			{
				vec3 WorldPoint{ 0,0,0 };
				D3DXVec3TransformCoord(&WorldPoint, &LocalPoint, &MWorld);
				IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), WorldPoint);
				if (IsRenderable)break;
			}

			//////////////////
			if (IsRenderable)
			{
				++RenderCount;
				RECT srcRect = { 0,0,static_cast<int32_t>(sp_TexInfo->ImageInfo.Width),static_cast<int32_t>(sp_TexInfo->ImageInfo.Height) };
				vec3 TextureCenter = { sp_TexInfo->ImageInfo.Width / 2.f,sp_TexInfo->ImageInfo.Height / 2.f,0.f };
				GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
				GraphicDevice::instance().GetSprite()->Draw(sp_TexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
					D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}
	}
}

void Terrain::Release()
{
	for (auto& _StateKey_LayerMap_TileVec : _TilesMap)
	{
		for (auto& _LayerMap_TileVec : _StateKey_LayerMap_TileVec.second)
		{
			auto& _TileVec = _LayerMap_TileVec.second;
			_TileVec.clear();
			_TileVec.shrink_to_fit();
		}

		_StateKey_LayerMap_TileVec.second.clear();
	}

	_TilesMap.clear();
}

void Terrain::SaveTilesCurrentStateKeyOnly(const std::wstring& FilePath)
{
	std::wofstream file_Input(FilePath);

	// 현재 맵 모드 의 모든 레이어 의 사이즈가 곧 타일의 개수
	size_t _InfoSize = std::accumulate(
		std::begin(_TilesMap[CurrentTileTextureStateKey]),
		std::end(_TilesMap[CurrentTileTextureStateKey]),
		0ul, [](size_t Size, const auto& LayerMap_RenderMapObjVec) {
		return LayerMap_RenderMapObjVec.second.size() + Size;
	});

	file_Input << _InfoSize << std::endl;

	for (const auto& LayerMap_RenderMapObjVec : _TilesMap[CurrentTileTextureStateKey])
	{
		for (const auto& _RenderMapObj : LayerMap_RenderMapObjVec.second)
		{
			file_Input << _RenderMapObj;
		}
	}
}

void Terrain::LoadTilesCurrentStateKeyOnly(const std::wstring& FilePath)
{
	std::wifstream file_output(FilePath);
	size_t _InfoSize{};
	file_output >> _InfoSize;
	Release();
	for (size_t i = 0; i < _InfoSize; ++i)
	{
		RenderMapObjInfo _RenderMapObjInfo;
		file_output >> _RenderMapObjInfo;

		_TilesMap[CurrentTileTextureStateKey][_RenderMapObjInfo._LayerMap].emplace_back(std::move(_RenderMapObjInfo));
	}
}


