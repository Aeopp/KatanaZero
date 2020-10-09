#pragma once
#include "Function.h"
#include <iostream>
#include "Typedef.h"

// TODO :: ZeroMemory �� ����ؼ� �ʱ�ȭ �ϴ� �ڵ尡 ����
// �����Ϳ� ����� ����ϰ� �;����ٸ� �����غ����Ѵ�.
struct TexInfo
{
	//�̹����� �����ϱ� ���� COM ��ü
	LPDIRECT3DTEXTURE9 pTexture; 

	// �̹����� ���� ������ ��� ���� ����ü. 
	D3DXIMAGE_INFO ImageInfo; 

	~TexInfo()noexcept{DXRelease(pTexture);}
};

typedef struct tagTexInfo
{
	// �̹��� ������ �����ϱ� ���� �İ�ü. ��¾�� ���� HBITMAP�� ������ ������ ������ ��. 
	LPDIRECT3DTEXTURE9 pTexture;

	// �̹����� ���� ������ ��� ���� ����ü. 
	D3DXIMAGE_INFO tImageInfo;
}TEXINFO;

// �� ������Ʈ�� �׸��� ���� ����
enum ELayer_Map  : int32_t
{
	BACK_GROUND=0,
	FORE_GROUND,
	DECORATIONS,
	NONE,
};

// ������ �ϱ����� �ʿ��� ����
struct RenderMapObjInfo
{
	ELayer_Map _LayerMap{ ELayer_Map::BACK_GROUND };
	// Texture ���� �׸��� ������
	vec3 Position{};
	int32_t DrawID{};
};

struct ObjectInfo
{
	enum EObjectID : uint32_t
	{
		NONE,
	}; 
	ObjectInfo() = default;
	ObjectInfo(const uint32_t _ObjectID, const vec3& Position) : _ObjectID{ _ObjectID }, Position{ Position } {};

	uint32_t _ObjectID{};
	vec3 Position{};
};
static std::wostream& operator<<(std::wostream& os,
	const ObjectInfo& _ObjectInfo)
{
	return os

		// ���� ���� ����
		<< _ObjectInfo._ObjectID << std::endl
		<< _ObjectInfo.Position << std::endl;
		// ���� ���� ��
}

static std::wistream& operator >> (std::wistream& is,
	ObjectInfo& _ObjectInfo)
{
	uint32_t temp{}; 

	is
		>> _ObjectInfo._ObjectID
		>> _ObjectInfo.Position;

	return is;
};



static std::wostream& operator<<(std::wostream& os,
	const RenderMapObjInfo& _RenderMapObjInfo)
{
	return os

		// ���� ���� ����
		<< _RenderMapObjInfo._LayerMap << std::endl
		<< _RenderMapObjInfo.Position << std::endl
		<< _RenderMapObjInfo.DrawID << std::endl;
	// ���� ���� ��
}

static std::wistream& operator >> (std::wistream& is,
	RenderMapObjInfo& _RenderMapObjInfo)
{
	int32_t _LayerMap{};

	 is
		>> _LayerMap
		>> _RenderMapObjInfo.Position
		>> _RenderMapObjInfo.DrawID;

	 _RenderMapObjInfo._LayerMap = static_cast<ELayer_Map>(_LayerMap);

	 return is;
};

struct UnitInfo
{
#ifdef _AFX
	CString Name;
#else
	wstring Name;
#endif // AFX
	int32_t Attack; 
	int32_t Defense;
	int32_t Job;
	int32_t Item;
};

static std::wostream& operator<<(std::wostream& os,
	const UnitInfo& _UnitInfo)
{
	return os

		// ������ ���ڿ� ��Ʈ�� üũ
		<< _UnitInfo.Name.GetString() <<std::endl
		<< _UnitInfo.Attack << std::endl
		<< _UnitInfo.Defense << std::endl
		<< _UnitInfo.Job << std::endl
		<< _UnitInfo.Item << std::endl;
}

static std::wistream& operator >> (std::wistream& is,
	UnitInfo& _UnitInfo)
{
	std::wstring Temp;

	is
		>> Temp
		>> _UnitInfo.Attack
		>> _UnitInfo.Defense
		>> _UnitInfo.Job
		>> _UnitInfo.Item;

	_UnitInfo.Name = Temp.c_str();

	return is;
};