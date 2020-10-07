#pragma once

class ObjectEdit
{
public:
	ObjectEdit();
	~ObjectEdit();
public:
	void Set_View(class CMFCToolView* pView) { this->pView = pView; }
public:
	// 패스받은 좌표의 현재 스테이지에 오브젝트 푸시
	void PushObjectInStage(const vec3 & Position/*Picking Target Position*/,const uint32_t DrawID /*DrawID 이면서 클라이언트에서 ObjectID로 파싱함*/);
	// 패스받은 좌표의 현재 스테이지에 있는 오브젝트삭제
	void DeleteObjAtPointLocation(const vec3& Position /*Target Position*/);
public:
	void Render();
	void Release();

	void SaveCurrentStageObjInfo(const std::wstring& FilePath);
	void LoadCurrentStageObjInfo(const std::wstring& FilePath);

	std::wstring CurrentEditStage;
	//           CurrentStageKey , ObjInfoVec
	std::map<std::wstring,std::vector<ObjectInfo>> _ObjectInfoMap;
private:
	class CMFCToolView* pView;
};

