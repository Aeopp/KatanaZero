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
	// ���̾ ������� �ʽ��ϴ�. �ش� ��ǥ�� ��� Ÿ���� �����˴ϴ�.
	void DeleteMapObjAtPointLocation(const vec3& Position /*Target Position*/);
	bool IsPicking(const vec3& TilePosition,const vec3& Point/*��κ��� ��Ȳ���� ���콺 ��ǥ*/);
public:
	// �����Լ� ȣ���� �ݵ�� �ؽ��� ������ƮŰ�� ������Ʈ ���ּ���.
	void DebugRender();
	void Render();
	void MiniRender();
	void Release(); 

	void SaveTilesCurrentStateKeyOnly(const std::wstring& FilePath);
	void LoadTilesCurrentStateKeyOnly(const std::wstring& FilePath);

	// ������ Ÿ�� ���������� �ٲ��� �ϴ� ��Ȳ�� �ݵ�� ȣ��
	std::wstring CurrentTileTextureStateKey;
			//TileStateKey
	std::map<std::wstring,std::map<ELayer_Map, vector<RenderMapObjInfo>>> _TilesMap;
private:
	std::vector<vec3> _DebugTiles;
	class CMFCToolView* pView;
};

