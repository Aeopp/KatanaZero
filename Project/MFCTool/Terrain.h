#pragma once

class Terrain
{
public:
	Terrain();
	~Terrain();
public:
	void Set_View(class CMFCToolView* pView) { this->pView = pView; }
public:
	void PickingPushMapObj(const vec3 & Position/*Picking Target Position*/,const int32_t DrawID,const ELayer_Map _LayerMap);
	// 레이어를 상관하지 않습니다. 해당 좌표의 모든 타일은 삭제됩니다.
	void DeleteMapObjAtPointLocation(const vec3& Position /*Target Position*/, const ELayer_Map _LayerMap);
	bool IsPicking(const vec3& TilePosition,const vec3& Point/*대부분의 상황에서 마우스 좌표*/);
public:
	// 렌더함수 호출전 반드시 텍스쳐 스테이트키를 업데이트 해주세요.
	void DebugRender();
	void Render();
	void MiniRender();
	void Release(); 

	void SaveTilesCurrentStateKeyOnly(const std::wstring& FilePath);
	void LoadTilesCurrentStateKeyOnly(const std::wstring& FilePath);

	// 논리적인 타일 스테이지가 바뀌어야 하는 상황에 반드시 호출
	std::wstring CurrentTileTextureStateKey;
			//TileStateKey
	std::map<std::wstring,std::map<ELayer_Map, vector<RenderMapObjInfo>>> _TilesMap;
private:
	std::vector<vec3> _DebugTiles;
	class CMFCToolView* pView;
};

