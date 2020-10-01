#include "stdafx.h"
#include "Terrain.h"
#include "MFCToolView.h"
#include "math.h"
#include <cassert>
#include <fstream>
#include <ostream>
#include <istream>
#include <numeric>


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
		_DebugTiles[i].x = ((SizeX) / 2) + ((i % TileNumY) * SizeX);
		_DebugTiles[i].y = ((SizeY) / 2) + ((i / TileNumX) * SizeY);
	}

	CurrentTileTextureStateKey = L"Prison";
}

Terrain::~Terrain()
{
	Release(); 
}

void Terrain::PickingPushMapObj(const vec3& Position,const int32_t DrawID, const ELayer_Map _LayerMap)
{
	for (auto & DebugTilePosition : _DebugTiles)
	{
		if (IsPicking(DebugTilePosition, Position))
		{
			RenderMapObjInfo _PushRenderMapObjInfo;
			_PushRenderMapObjInfo.Position = DebugTilePosition;
			_PushRenderMapObjInfo.DrawID = DrawID;
			_PushRenderMapObjInfo._LayerMap = _LayerMap;

			// ���̾ ��� ���� ������ ��ġ�� ���� Ÿ���� �ߺ� ���ԵǴ� ���� �����մϴ�.
			for (const auto& _LayerMap_RenderMapObjVec : _TilesMap[CurrentTileTextureStateKey])
			{
				const auto& _RenderMapObjVec = _LayerMap_RenderMapObjVec.second;

				auto is_find = std::find_if(std::begin(_RenderMapObjVec), std::end(_RenderMapObjVec),
					[DrawID, DebugTilePosition](const auto& _RenderMapObj)
				{
					return _RenderMapObj.DrawID == DrawID &&
							_RenderMapObj.Position == DebugTilePosition;
				});

				if (is_find != std::end(_RenderMapObjVec))
					return;
			}
			//////////////////////////////////////

			_TilesMap[CurrentTileTextureStateKey][_LayerMap].
				emplace_back(std::move(_PushRenderMapObjInfo));

				return;
		}
	}
}

void Terrain::DeleteMapObjAtPointLocation(const vec3 & Position)
{

	// ���̾� ��� ���� Ÿ�� �������� ��� Ÿ�� ����

	for (auto& _LayerMap_RenderMapObjVec : _TilesMap[CurrentTileTextureStateKey])
	{
		auto& _RenderMapObjVec = _LayerMap_RenderMapObjVec.second;

		auto erase_begin = std::remove_if(std::begin(_RenderMapObjVec),
			std::end(_RenderMapObjVec), [this,Position](auto& _RenderMapObj)
		{
			return IsPicking(_RenderMapObj.Position, Position);
		});

		_RenderMapObjVec.erase(erase_begin, std::end(_RenderMapObjVec));
		_RenderMapObjVec.shrink_to_fit();
	}
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

	constexpr float DebugLineWidth = 0.5f;

	assert(pView&&__FUNCTIONW__);

	matrix MScale, MTranslation, MWorld;

	const int32_t SizeX_half = global::TileSize.first  / 2;
	const int32_t SizeY_half = global::TileSize.second / 2;

	GraphicDevice::instance().GetSprite()->End();

	for (const auto& DebugTile : _DebugTiles)
	{
		vec3 RenderPos = DebugTile;

		RenderPos -= vec3{ static_cast<float> (pView->GetScrollPos(0)),
						   static_cast<float>( pView->GetScrollPos(1)),0.f };
	
		std::array<D3DXVECTOR2 ,5> LineVertexs;

		LineVertexs[0] = { RenderPos.x- SizeX_half, RenderPos.y- SizeY_half };
		LineVertexs[1] = { RenderPos.x+ SizeX_half, RenderPos.y- SizeY_half };
		LineVertexs[2] = { RenderPos.x+ SizeX_half, RenderPos.y+ SizeY_half };
		LineVertexs[3] = { RenderPos.x- SizeX_half, RenderPos.y+ SizeY_half };
		LineVertexs[4] = { RenderPos.x- SizeX_half, RenderPos.y- SizeY_half };
		
		GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);
		GraphicDevice::instance().GetLine()->Draw(LineVertexs.data(), LineVertexs.size(),
		(D3DCOLOR_ARGB(147, 255, 170, 165)));
	}

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void Terrain::Render()
{
	matrix MScale,MWorld;
	D3DXMatrixScaling(&MScale, 1.f, 1.f, 0.f);

	for (auto& layer_TileVec: _TilesMap[CurrentTileTextureStateKey])
	{
		for (auto& MapObj : layer_TileVec.second)
		{
			matrix MTrans;
			// TODO :: ������ ������ �ִ� �ڵ� => DrawID �� MapObj �� �ѹ��� 
			//�׷����� �����شٸ� ȣ��Ƚ���� ���� �پ���
			// �������ϸ� ���� ���� ���
			auto sp_TexInfo = Texture_Manager::instance().Get_TexInfo(L"Map", 
			CurrentTileTextureStateKey, MapObj.DrawID);
			if (!sp_TexInfo)continue; 
		
			D3DXMatrixTranslation(&MTrans, 

				MapObj.Position.x - pView->GetScrollPos(0)+ 
					sp_TexInfo->ImageInfo.Width/2 - global::TileSize.first/2, 

				MapObj.Position.y- pView->GetScrollPos(1) + 
					sp_TexInfo->ImageInfo.Height/2-global::TileSize.second/ 2, 0.f);

				MWorld = MScale* MTrans;

			CRect srcRect = { 0,0,static_cast<int32_t>(sp_TexInfo->ImageInfo.Width),static_cast<int32_t>(sp_TexInfo->ImageInfo.Height) };
			vec3 TextureCenter = { sp_TexInfo->ImageInfo.Width/2.f,sp_TexInfo->ImageInfo.Height/2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(sp_TexInfo->pTexture, srcRect, &TextureCenter, nullptr, 
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}

	DebugRender();
}

void Terrain::MiniRender()
{
	constexpr float MiniRenderViewScale = 0.7f;

	matrix MScale, MWorld;

	D3DXMatrixScaling(&MScale, MiniRenderViewScale, MiniRenderViewScale, 0.f);

	for (auto& layer_TileVec : _TilesMap[CurrentTileTextureStateKey])
	{
		for (auto& MapObj : layer_TileVec.second)
		{
			matrix MTrans;
			// TODO :: ������ ������ �ִ� �ڵ� => DrawID �� MapObj �� �ѹ��� 
			//�׷����� �����شٸ� ȣ��Ƚ���� ���� �پ���
			// �������ϸ� ���� ���� ���
			auto sp_TexInfo = Texture_Manager::instance().Get_TexInfo(L"Map",
				CurrentTileTextureStateKey, MapObj.DrawID);
			if (!sp_TexInfo)continue;

			D3DXMatrixTranslation(&MTrans, MapObj.Position.x - pView->GetScrollPos(0),
				MapObj.Position.y - pView->GetScrollPos(1), 0.f);
			MWorld =  MTrans*MScale;

			CRect srcRect = { 0,0,static_cast<int32_t>(sp_TexInfo->ImageInfo.Width),static_cast<int32_t>(sp_TexInfo->ImageInfo.Height) };
			vec3 TextureCenter = { sp_TexInfo->ImageInfo.Width / 2.f,sp_TexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(sp_TexInfo->pTexture, srcRect, &TextureCenter, nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}

void Terrain::Release()
{
	for (auto& _StateKey_LayerMap_TileVec : _TilesMap)
	{
		for (auto& _LayerMap_TileVec : _StateKey_LayerMap_TileVec.second)
		{
			auto& _TileVec =  _LayerMap_TileVec.second;
				_TileVec .clear();
				_TileVec .shrink_to_fit();
		}

		_StateKey_LayerMap_TileVec.second.clear();
	}

	_TilesMap.clear(); 
}

void Terrain::SaveTilesCurrentStateKeyOnly(const std::wstring& FilePath)
{
	std::wofstream file_Input(FilePath);

	// ���� �� ��� �� ��� ���̾� �� ����� �� Ÿ���� ����
	size_t _InfoSize = std::accumulate(
		std::begin(_TilesMap[CurrentTileTextureStateKey]),
		std::end(_TilesMap[CurrentTileTextureStateKey]),
		0ul,[](size_t Size,const auto& LayerMap_RenderMapObjVec){
		return LayerMap_RenderMapObjVec.second.size() + Size;
	});

	file_Input << _InfoSize << std::endl;

	for (const auto& LayerMap_RenderMapObjVec:_TilesMap[CurrentTileTextureStateKey])
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

	for (size_t i = 0; i < _InfoSize; ++i)
	{
		RenderMapObjInfo _RenderMapObjInfo;
		file_output >> _RenderMapObjInfo;

		_TilesMap[CurrentTileTextureStateKey][_RenderMapObjInfo._LayerMap].emplace_back(std::move(_RenderMapObjInfo));
	}
}
