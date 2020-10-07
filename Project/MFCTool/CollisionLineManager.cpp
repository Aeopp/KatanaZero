#include "stdafx.h"
#include "CollisionLineManager.h"
#include "math.h"
#include "MFCToolView.h"
#include "MainFrm.h"
#include "Graphic_Device.h"
#include <fstream>
#include <ostream>
#include <istream>

void CollisionLineManager::Push(std::pair<vec3, vec3> Line) &
{
	_CollisionLineMap[CurrentStateKey].push_back(std::move(Line));
}

void CollisionLineManager::Erase(const vec3 & TargetPosition) &
{
	auto& CurrentCollisionLineVec = _CollisionLineMap[CurrentStateKey];

	 auto IsFindIter = std::find_if(std::begin(CurrentCollisionLineVec),
								   std::end(CurrentCollisionLineVec),
		[TargetPosition](const auto& CollisionLine)
	{
		constexpr float DistanceMin = 20.f;
		return (
				DistanceMin > math::GetPointDistance({ TargetPosition,CollisionLine.first })
			||
				DistanceMin > math::GetPointDistance({ TargetPosition,CollisionLine.second }));
	});

	if (IsFindIter != std::end(CurrentCollisionLineVec))
		CurrentCollisionLineVec.erase(IsFindIter);
}

void CollisionLineManager::DebugRender()  &
{
	if (!global::bDebug)return;

	constexpr float DebugLineWidth = 3.f;
	constexpr auto DebugLineColor = D3DCOLOR_ARGB(255, 0, 0, 255);

	std::pair<float, float > CameraPos{ 0.f,0.f };
	float JoomScale = 1.f;
	matrix MJoom;
#ifdef _AFX
	CMainFrame*pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (!pMain)return;
	CMFCToolView*pView = dynamic_cast<CMFCToolView*>(pMain->_MainSplitter.GetPane(0, 1));
	if (!pView)return;

	CameraPos.first = pView->GetScrollPos(0);
	CameraPos.second = pView->GetScrollPos(1);
	JoomScale = pView->JoomScale;
#endif
	GraphicDevice::instance().GetSprite()->End();
	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);


	MJoom = math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,
		global::ClientSize.second,0.f });
	uint32_t RenderCount = 0;

	const auto& CurrentCollisionLineVec = _CollisionLineMap[CurrentStateKey];

	for (const auto& CollisionLine : CurrentCollisionLineVec)
	{
		std::array<vec2, 2> Line2Ds;

		Line2Ds[0] ={ CollisionLine.first.x - CameraPos.first  ,
					CollisionLine.first.y- CameraPos.second };

		Line2Ds[1] = { CollisionLine.second.x - CameraPos.first,
			CollisionLine.second.y - CameraPos.second };

		D3DXVec2TransformCoord(&Line2Ds[0], &Line2Ds[0], &MJoom);
		D3DXVec2TransformCoord(&Line2Ds[1], &Line2Ds[1], &MJoom);

		bool IsRenderable = false;

		IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), vec3{ Line2Ds[0].x,Line2Ds[0].y,0.f });
		IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), vec3{ Line2Ds[1].x,Line2Ds[1].y,0.f });

		if (IsRenderable)
		{
			RenderCount++;
			GraphicDevice::instance().GetLine()->Draw(Line2Ds.data(),
				Line2Ds.size(), DebugLineColor);
		}
	}

	std::wstring DebugTileStr = L"RenderDebugLine : " + std::to_wstring(RenderCount);
	RECT rectRender{ 0,100,500,125 };
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	DebugTileStr = L"CullingRenderDebugLine : " + std::to_wstring(CurrentCollisionLineVec.size() - RenderCount);
	rectRender = { 0,125,500,150};
	GraphicDevice::instance().GetFont()->DrawTextW(nullptr, DebugTileStr.c_str(), DebugTileStr.size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void CollisionLineManager::LoadCollisionLine(const std::wstring & FilePath) &
{
	std::wifstream file_output(FilePath);
	size_t _InfoSize{};
	file_output >> _InfoSize;

	 auto& CurrentCollisionLineVec = _CollisionLineMap[CurrentStateKey];

	for (size_t i = 0; i < _InfoSize; ++i)
	{
		typename std::decay_t<decltype(CurrentCollisionLineVec)>::value_type _CollisionLine;

		file_output >> _CollisionLine.first; 
		file_output >> _CollisionLine.second;

		CurrentCollisionLineVec.push_back(std::move(_CollisionLine));
	}
}

void CollisionLineManager::SaveCollisionLine(const std::wstring & FilePath) &
{
	std::wofstream file_Input(FilePath);

	const auto& CurrentCollisionLineVec = _CollisionLineMap[CurrentStateKey];

	size_t _InfoSize = CurrentCollisionLineVec.size();

	file_Input << _InfoSize << std::endl;

	for (const auto& CollisionLine : CurrentCollisionLineVec)
	{
		file_Input << CollisionLine.first;
		file_Input << CollisionLine.second;
	}
}

void CollisionLineManager::Clear() &
{
	_CollisionLineMap.clear();
}
