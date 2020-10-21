#include "stdafx.h"
#include "RenderComponent.h"
#include "math.h"
#include "TransformComponent.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"

void RenderComponent::Render()
{
	Component::Render();

	auto spOwner = _Owner.lock();
	auto spTexInfo = TextureManager::instance().Get_TexInfo(_RenderInfo.ObjectKey, _RenderInfo.StateKey, _RenderInfo.Number);

	if (!spOwner)return;
	if (!spOwner->_TransformComp)return;
	if (!spTexInfo)return;

	const float JoomScale = global::JoomScale;
	vec3 CameraPos = global::CameraPos;

	matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(true);

	MWorld = MWorld * math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

	const auto LocalPoints = math::GetLocalRect(vec2{ (float)spTexInfo->ImageInfo.Width,(float)spTexInfo->ImageInfo.Height });

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
		RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width,spTexInfo->ImageInfo.Height };
		vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
		GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
		GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}

}
