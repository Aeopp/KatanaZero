#include "stdafx.h"
#include "RenderComponent.h"
#include "math.h"
#include "Time.h"

#include "TransformComponent.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "Player.h"
#include "PhysicTransformComponent.h"
#include <sstream>

void RenderComponent::Render()
{
	if (!bRender)return;

	Component::Render();

	auto spOwner = _Owner.lock();
	auto spTexInfo = TextureManager::instance().Get_TexInfo(_Info.ObjectKey, _Info.StateKey, _Info.CurrentFrame);

	if (!spOwner)return;
	if (!spOwner->_TransformComp)return;
	if (!spTexInfo)return;

	const float JoomScale = global::JoomScale;
	vec3 CameraPos = global::CameraPos;

	matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(true);

	MWorld = MWorld * math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });
	MWorld._11 *= AnimDir;
	MWorld._41 += PositionCorrection.x;
	MWorld._42 += PositionCorrection.y;
	MWorld._43 += PositionCorrection.z;

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
		RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width *_Info.SrcScale.x,
							  spTexInfo->ImageInfo.Height * _Info.SrcScale.y };
		vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,spTexInfo->ImageInfo.Height / 2.f,0.f };
		GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
	
		if (bAfterImg)
		{
			for (auto& _AfterImg : _AfterQ)
			{
				auto TexInfo = TextureManager::instance().Get_TexInfo(_Info.ObjectKey, _AfterImg.StateKey, _AfterImg.ID);
				RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width * _Info.SrcScale.x,
							  TexInfo->ImageInfo.Height * _Info.SrcScale.y };
				vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
				vec3 v = _AfterImg.PastPosition - spOwner->_TransformComp->Position;

				GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture, &_srcRT, &TextureCenter,
					&(v * 0.04f), _AfterImg._Color);
			}

			AfterImg _AfterImg;
			_AfterImg.ID = _Info.CurrentFrame;
			_AfterImg.StateKey = _Info.StateKey;
			_AfterImg.PastPosition = spOwner->_TransformComp->Position;
			_AfterImg._Color = AfterImg::_ColorTable[AfterImg::CurColorIdx++]; 
			AfterImg::CurColorIdx %= AfterImg::_ColorTable.size();
			_AfterQ.push_back(std::move(_AfterImg));
			if (_AfterQ.size() > 15)
				_AfterQ.pop_front();
		}

		GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, &srcRect, &TextureCenter, nullptr,
			_Info._Color);
	}



	auto NotifyEvent = _Info._Nofify.find(_Info.CurrentFrame);

	if (NotifyEvent != std::end(_Info._Nofify))
	{
		if (NotifyEvent->second)
		{
			// 해당 프레임에 도달하였으니 함수 호출
			NotifyEvent->second();
		}
	}

	if (spOwner->GetID() == OBJECT_ID::EID::EPLAYER)
	{
		auto _Player = std::dynamic_pointer_cast<Player>(spOwner);
		_Player->bCurWallRideCollision;
		auto _PhysicComp = std::dynamic_pointer_cast<PhysicTransformComponent>(_Player->_TransformComp);
		_PhysicComp->bLand;
		_PhysicComp->bFly;
		std::wstringstream wss;
		wss << L"WALL_RIDE : " << _Player->bCurWallRideCollision << L" LANDING : " << _PhysicComp->bLand << L" Flying : " << _PhysicComp->bFly << std::endl;
		RECT rectRender{ 1400,200,2000,850 };
		GraphicDevice::instance().GetFont()->DrawTextW(nullptr, wss.str().c_str(), wss.str().size(), &rectRender, 0, D3DCOLOR_ARGB(255, 109, 114, 255));
	}
}

void RenderComponent::Update()
{
	Component::Update();
	const float Dt = Time::instance().Delta();
	
	if ( ! (_Info.T < _Info.End) )
	{
		auto NotifyEvent=_Info._Nofify.find(_Info.End);

		if (NotifyEvent != std::end(_Info._Nofify))
		{
			if (NotifyEvent->second)
			{
				// 해당 프레임에 도달하였으니 함수 호출
				NotifyEvent->second();
			}
		}

		if (_Info.bLoop)
		{
			_Info.T -= _Info.End;
		}
		else if (!_Info.bLoop)
		{
			_Info.T = (_Info.End - 1);
			_Info.T -= Dt * (_Info.End / _Info.AnimSpeed);
		}
	}

	_Info.T += Dt * (_Info.End / _Info.AnimSpeed);/*스프라이트 개수가 많고 애니메이션 스피드가 짧을수록 가속*/
	_Info.CurrentFrame = (std::min<uint8_t>)(_Info.T, _Info.End - 1);
}

std::optional<float>RenderComponent::CalcY()
{
	std::optional<float> opY = std::nullopt;

	auto spOwner =_Owner.lock();
	if (!spOwner)return opY;
	if (!spOwner->_TransformComp)return opY;

	auto spTexInfo = TextureManager::instance().Get_TexInfo(_Info.ObjectKey, _Info.StateKey, _Info.GetCurFrame());
	if (!spTexInfo)return opY;

	opY=spOwner->_TransformComp->Position.y + ((float)spTexInfo->ImageInfo.Height / 2.f);
	return opY;
}

void RenderComponent::Anim(
	const bool SamePlayAgain,
	const bool bLoop, 
	const std::wstring& StateKey, 
	const uint8_t End, 
	const float AnimSpeed, 
	typename RenderComponent::NotifyType _Nofify, 
	const D3DCOLOR _Color, 
	const float StartT,
	vec2 SrcScale, 
	const std::wstring& ObjKey,
	LAYER::ELAYER _Layer)
{
	if (!SamePlayAgain && StateKey==_Info.StateKey)return;

	_Info.bLoop = bLoop;
	_Info.AnimSpeed = AnimSpeed;
	_Info.CurrentFrame = StartT;
	_Info.End = End;
	_Info.ObjectKey = ObjKey == L"" ? _Info.ObjectKey : ObjKey;
	_Info.SrcScale = SrcScale.x == -1.f ? _Info.SrcScale : SrcScale;
	_Info.StateKey = StateKey;
	_Info.T = StartT;
	_Info._Color = _Color == D3DCOLOR_ARGB(255, 255, 255, 255) ?	_Info._Color : _Color;
	_Info._Layer = _Layer == LAYER::ELAYER::ENONE ? _Info._Layer : _Layer;
	_Info._Nofify.clear();
	_Info._Nofify = std::move(_Nofify);
}
