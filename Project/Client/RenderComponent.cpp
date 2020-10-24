#include "stdafx.h"
#include "RenderComponent.h"
#include "math.h"
#include "TransformComponent.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "Player.h"
#include "PhysicTransformComponent.h"
#include <sstream>




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
		RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width *_RenderInfo.SrcScale.x,
							  spTexInfo->ImageInfo.Height * _RenderInfo.SrcScale.y };
		vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
		GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
		GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
			_RenderInfo._Color);
	}

	
	if (spOwner->GetID() == OBJECT_ID::EID::EPLAYER)
	{
		auto _Player = std::dynamic_pointer_cast<Player>(spOwner);
		_Player->bWallRide;
		auto _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_Player->_TransformComp);
		_PhysicComp->bLand;
		_PhysicComp->bFly;
		std::wstringstream wss;
		wss << L"WALL_RIDE : " << _Player->bWallRide << L" LANDING : " << _PhysicComp->bLand << L" Flying : " << _PhysicComp->bFly << std::endl;
		RECT rectRender{ 1400,200,2000,850 };
		GraphicDevice::instance().GetFont()->DrawTextW(nullptr, wss.str().c_str(), wss.str().size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));

	}
}

std::optional<float>RenderComponent::CalcY()
{
	std::optional<float> opY = std::nullopt;

	auto spOwner =_Owner.lock();
	if (!spOwner)return opY;
	if (!spOwner->_TransformComp)return opY;

	auto spTexInfo = TextureManager::instance().Get_TexInfo(_RenderInfo.ObjectKey, _RenderInfo.StateKey, _RenderInfo.Number);
	if (!spTexInfo)return opY;

	opY=spOwner->_TransformComp->Position.y + ((float)spTexInfo->ImageInfo.Height / 2.f);
	return opY;
}
