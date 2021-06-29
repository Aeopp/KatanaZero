#include "stdafx.h"
#include "UIRenderComponent.h"
#include "Texture_Manager.h"
#include "math.h"
#include "TransformComponent.h"
#include "GraphicDevice.h"

void UIRenderComponent::Render()
{
	Component::Render();

	auto spOwner = _Owner.lock();
	auto spTexInfo = TextureManager::instance().Get_TexInfo(_Info.ObjectKey, _Info.StateKey, _Info.GetCurFrame());
	
	if (!spOwner)return;
	if (!spOwner->_TransformComp)return;
	if (!spTexInfo)return;

	const float JoomScale = global::JoomScale;
	vec3 CameraPos = global::CameraPos;

	matrix MWorld = spOwner->_TransformComp->CalcSTWorldMatrix(true);
	
	RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _Info.SrcScale.x,
							  spTexInfo->ImageInfo.Height * _Info.SrcScale.y };
	vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
	GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
	GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
	  _Info._Color);

	if (_RenderAfterEvent)
	{
		_RenderAfterEvent();
	}
}
