#include "stdafx.h"
#include "MFCTool.h"
#include "ObjectEdit.h"
#include "MFCToolView.h"
#include "math.h"
#include <cassert>
#include <fstream>
#include <ostream>
#include <istream>
#include <numeric>
#include "MainFrm.h"
#include "MyForm.h"

// CItem
ObjectEdit::ObjectEdit()
{
	CurrentEditStage = L"Prison";
}

ObjectEdit::~ObjectEdit()
{
	Release();
}

void ObjectEdit::PushObjectInStage(const vec3 & Position, const uint32_t DrawID)
{
	_ObjectInfoMap[CurrentEditStage].emplace_back(DrawID,Position );
}

void ObjectEdit::DeleteObjAtPointLocation(const vec3 & Position)
{
	auto EraseBeginIter = std::remove_if(std::begin(_ObjectInfoMap[CurrentEditStage]), std::end(_ObjectInfoMap[CurrentEditStage]), 
	[Position](const auto& _ObjectInfo) {
		static constexpr float PermitDistance = 20.f;
		vec3 Distance = _ObjectInfo.Position - Position;
		return D3DXVec3Length(&Distance) < PermitDistance;
	});
	
	_ObjectInfoMap[CurrentEditStage].erase(EraseBeginIter, std::end(_ObjectInfoMap[CurrentEditStage]));
}

void ObjectEdit::Render()
{
#ifdef _AFX
	assert(pView&&__FUNCTION__);
#endif // _AFX
	matrix MScale, MWorld;
	D3DXMatrixScaling(&MScale, 1.f, 1.f, 0.f);

	uint32_t RenderCount = 0;

	uint32_t CurrentStageObjCount = _ObjectInfoMap[CurrentEditStage].size();

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = dynamic_cast<CMyForm*>(pMain->_SecondSplitter.GetPane(1, 0));

	for (auto& CurrentObjInfo : _ObjectInfoMap[CurrentEditStage])
	{
		matrix MTrans;
		auto sp_TexInfo = TextureManager::instance().Get_TexInfo(L"EditObject", L"PlacePicture", CurrentObjInfo._ObjectID);

		if (!sp_TexInfo)continue;

		D3DXMatrixTranslation(&MTrans,

			CurrentObjInfo.Position.x - pView->GetScrollPos(0) +
			sp_TexInfo->ImageInfo.Width / 2 - global::TileSize.first / 2,

			CurrentObjInfo.Position.y - pView->GetScrollPos(1) +
			sp_TexInfo->ImageInfo.Height / 2 - global::TileSize.second / 2, 0.f);

		MWorld = MScale* MTrans  * math::GetCameraJoomMatrix(pView->JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

		auto LocalPoints = math::GetLocalRect(vec2{ (float)sp_TexInfo->ImageInfo.Width,(float)sp_TexInfo->ImageInfo.Height });

		bool IsRenderable = false;

		for (auto& LocalPoint : LocalPoints)
		{
			D3DXVec3TransformCoord(&LocalPoint, &LocalPoint, &MWorld);
			IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), LocalPoint);
			if (IsRenderable)break;
		}

		//////////////////
		if (IsRenderable)
		{
			++RenderCount;
			RECT srcRect = { 0,0,static_cast<int32_t>(sp_TexInfo->ImageInfo.Width),static_cast<int32_t>(sp_TexInfo->ImageInfo.Height) };
			vec3 TextureCenter = { sp_TexInfo->ImageInfo.Width / 2.f,sp_TexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(sp_TexInfo->pTexture, &srcRect, &TextureCenter, nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}

void ObjectEdit::Release()
{
	_ObjectInfoMap.clear();
}

void ObjectEdit::SaveCurrentStageObjInfo(const std::wstring & FilePath)
{
	std::wofstream file_Input(FilePath);

	size_t _InfoSize = _ObjectInfoMap[CurrentEditStage].size();

	file_Input << _InfoSize << std::endl;
	
	for (const auto& _CurrentObjInfo: _ObjectInfoMap[CurrentEditStage])
	{
		file_Input << _CurrentObjInfo;
	}
}

void ObjectEdit::LoadCurrentStageObjInfo(const std::wstring & FilePath)
{
	std::wifstream file_output(FilePath);
	size_t _InfoSize{};
	file_output >> _InfoSize;

	for (size_t i = 0; i < _InfoSize; ++i)
	{
		ObjectInfo _ObjectInfo;
		file_output >> _ObjectInfo;

		_ObjectInfoMap[CurrentEditStage].emplace_back(std::move(_ObjectInfo));
	}
}