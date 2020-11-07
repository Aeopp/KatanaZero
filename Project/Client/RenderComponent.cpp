#include "stdafx.h"
#include "RenderComponent.h"
#include "math.h"
#include "Time.h"
#include "TransformComponent.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "Player.h"
#include "PhysicTransformComponent.h"
#include "RecordManager.h"
#include <sstream>

void RenderComponent::RecordRender()
{
	_Record.Render();
}

void RenderComponent::Render()
{
	if (!bRender)return;

	const float dt = Time::instance().Delta();

	Component::Render();

	auto spOwner = _Owner.lock();
	auto spTexInfo = TextureManager::instance().Get_TexInfo
	(_Info.ObjectKey, _Info.StateKey, _Info.CurrentFrame);

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
	CurRenderWorld = MWorld;

	const auto LocalPoints = math::GetLocalRect(vec2{ (float)spTexInfo->ImageInfo.Width,(float)spTexInfo->ImageInfo.Height });

	bool IsRenderable = false;
	
	for (const auto& LocalPoint : LocalPoints)
	{
		vec3 WorldPoint{ 0,0,0 };
		D3DXVec3TransformCoord(&WorldPoint, &LocalPoint, &MWorld);
		IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), WorldPoint);
		if (IsRenderable)break;
	}

	if (IsRenderable)
	{
		//if (bAfterRender)
		{
			for (auto _AfterIter = begin(_AfterImgVec);
				_AfterIter != end(_AfterImgVec);)
			{
				auto& _After = *_AfterIter;
				
				auto TexInfo = TextureManager::instance().
					Get_TexInfo(/*_Info.ObjectKey*/_After.ObjKey, _After.StateKey, _After.ID);
				RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width * _Info.SrcScale.x,
							  TexInfo->ImageInfo.Height * _Info.SrcScale.y };
				vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
				GraphicDevice::instance().GetSprite()->SetTransform(&_After.PastWorld);
				GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
					&_srcRT, &__TextureCenter, nullptr,
					_After._Color);
				

				_After.T += _After.DeltaCoefft * dt;
				D3DXColorLerp(&_After._Color, &_After._Color, &_After._GoalColor, _After.T);

				if (_After._Color.a < 0)
				{
					_AfterIter =_AfterImgVec.erase(_AfterIter);
				}
					else ++_AfterIter;
			}
			if (!spOwner->bSmoke)
			{
				AfterImgPush(MWorld);
			}
		}

		if (global::ECurGameState::PlaySlow == global::_CurGameState && bSlowRender)
		{

			RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _Info.SrcScale.x,
					  spTexInfo->ImageInfo.Height * _Info.SrcScale.y };
			vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,
			spTexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture, 
			&srcRect, &TextureCenter, nullptr,
				SlowColor);

			// 그려지는 상황이므로 푸시
			if (global::IsPlay() && RecordManager::instance().bUpdate)
			{
				Record::Info _RecordInfo;
				int32_t CurTiming = RecordManager::instance().Timing;
				_RecordInfo.Alpha = 255;
				_RecordInfo.DrawID = _Info.CurrentFrame;
				_RecordInfo.MWorld = MWorld;
				_RecordInfo.ObjKey = _Info.ObjectKey;
				_RecordInfo.StateKey = _Info.StateKey;
				_RecordInfo.OwnerY = spOwner->_TransformComp->Position.y;
				_RecordInfo.Timing = CurTiming;
				_RecordInfo._Color = _Info._Color;

				_Record._Infos.insert({ CurTiming  , _RecordInfo });
			}
		}
		else
		{
			D3DXCOLOR _Color = _Info._Color;
			if (spOwner->bSmoke && bSmokeAffected)
			{
				_Color = D3DCOLOR_ARGB(255, 0, 0, 0);
			}
			RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _Info.SrcScale.x,
					  spTexInfo->ImageInfo.Height * _Info.SrcScale.y };
			vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,
			spTexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture,
				&srcRect, &TextureCenter, nullptr,
				_Color);

			// 그려지는 상황이므로 푸시
			if (global::IsPlay() && RecordManager::instance().bUpdate)
			{
				Record::Info _RecordInfo;
				int32_t CurTiming = RecordManager::instance().Timing; 
				_RecordInfo.Alpha =           255;
				_RecordInfo.DrawID=	    _Info.CurrentFrame;
				_RecordInfo.MWorld=	      MWorld;
				_RecordInfo.ObjKey=           _Info.ObjectKey;
				_RecordInfo.StateKey=           _Info.StateKey;
				_RecordInfo.OwnerY=             spOwner->_TransformComp->Position.y;
				_RecordInfo.Timing = CurTiming;
				_RecordInfo._Color = _Color;

				_Record._Infos.insert({ CurTiming  , _RecordInfo});
			}
		}
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

	if (_RenderAfterEvent)
	{
		_RenderAfterEvent();
	}

	/*if (spOwner->GetID() == OBJECT_ID::EID::EPLAYER)
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
	}*/
}

void RenderComponent::Update()
{
	Component::Update();
	const float Dt = Time::instance().Delta();

	SlowAfterImgPushCurrentDelta -= Dt;
	NormalAfterImgPushCurrentDelta -= Dt;

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
	if (!global::IsPlay())return;

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

void RenderComponent::AfterImgOn()
{
	bAfterRender = true;
}

void RenderComponent::AfterImgOff()
{
	bAfterRender = false;
}

void RenderComponent::AfterImgPush(matrix MWorld)
{
	global::_CurGameState;
	AfterImg _AfterImg;

	switch (global::_CurGameState)
	{
	case global::ECurGameState::PlaySlow:
		if (SlowAfterImgPushCurrentDelta < 0.f && bSlowRender)
		{
			SlowAfterImgPushCurrentDelta = SlowAfterImgPushDelta;
			_AfterImg.ID = _Info.CurrentFrame;
			_AfterImg.StateKey = _Info.StateKey;
			_AfterImg.PastWorld = (MWorld);
			_AfterImg._Color = SlowStartColor;
			_AfterImg._GoalColor = SlowGoalColor;
			_AfterImg.ObjKey = _Info.ObjectKey;
			_AfterImg.T = 0.f;
			_AfterImg.DeltaCoefft = SlowDeltaCoefft;
			_AfterImgVec.push_back(std::move(_AfterImg));
		}
	//	AfterCheckingPush( MWorld);
		break;
	case global::ECurGameState::Play:
		AfterCheckingPush(MWorld);
		break;
	default:
		break;
	}
}

D3DXCOLOR RenderComponent::GetCurGameStateColor()
{
	if (!bAfterRender)return _Info._Color;

	switch (global::_CurGameState)
	{
	case global::ECurGameState::Play:
		return _Info._Color;
		break;	
	case global::ECurGameState::PlaySlow:
		return SlowStartColor;
		break;
	case global::ECurGameState::Replay : 
		return ReplayColor;
		break;
	default:
		break;
	}
	return _Info._Color;
}

void RenderComponent::AfterCheckingPush(
	matrix MWorld)
{
	AfterImg _AfterImg;

	if ((NormalAfterImgPushCurrentDelta < 0.f) && bAfterRender)
	{
		NormalAfterImgPushCurrentDelta = NormalAfterImgPushDelta;
		_AfterImg.ID = _Info.CurrentFrame;
		_AfterImg.StateKey = _Info.StateKey;
		_AfterImg.PastWorld = std::move(MWorld);
		_AfterImg._Color = PlayStartColor;
		_AfterImg._GoalColor = PlayGoalColor;
		_AfterImg.T = 0.f;
		_AfterImg.DeltaCoefft = AfterDeltaCoefft;
		_AfterImg.ObjKey = _Info.ObjectKey;
		_AfterImgVec.push_back(std::move(_AfterImg));
	}
}

