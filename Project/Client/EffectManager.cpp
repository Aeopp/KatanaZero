#include "stdafx.h"
#include "EffectManager.h"
#include "Time.h"
#include "math.h"
#include "GraphicDevice.h"
#include "Texture_Manager.h"
#include "ComponentManager.h"
#include "RecordManager.h"

void EffectManager::Initialize() & noexcept
{
	_EffectCollisionTagSet.insert(CollisionComponent::ETag::EEnemy);
	_EffectCollisionTagSet.insert(CollisionComponent::ETag::EPlayer);
	_EffectCollisionTagSet.insert(CollisionComponent::ETag::EPlayerAttack);
	_EffectCollisionTagSet.insert(CollisionComponent::ETag::EItem);
	_EffectCollisionTagSet.insert(CollisionComponent::ETag::EInteractionObject);
	_EffectCollisionTagSet.insert(CollisionComponent::ETag::ESmoke_Cloud);
};

void EffectManager::Render()
{
	if (!bRender)return;

	const float dt = Time::instance().Delta();
	const float JoomScale = global::JoomScale;
	const int32_t Timing = RecordManager::instance().Timing;

	vec3 CameraPos = global::CameraPos;

	std::sort(std::begin(_Effects), std::end(_Effects), [](const EffectInfo& _EftLhs, const EffectInfo& _EftRhs)->bool {
		return _EftLhs.Layer < _EftRhs.Layer;
	});

	for (auto& _Effect : _Effects)
	{
		matrix MWorld, MTrans, MRotZ, MScale, MJoom;

		D3DXMatrixScaling(&MScale, _Effect.Scale.x, _Effect.Scale.y, _Effect.Scale.z);
		D3DXMatrixRotationZ(&MRotZ, _Effect.RotZ);
		D3DXMatrixTranslation(&MTrans, _Effect.Pos.x - CameraPos.x, _Effect.Pos.y - CameraPos.y, _Effect.Pos.z);

		MJoom = math::GetCameraJoomMatrix(JoomScale,
			vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

		MWorld = MScale * MRotZ * MTrans * MJoom;

		D3DXCOLOR _Color;

		if (!_Effect.bAlphaLerp)
			_Color = D3DCOLOR_ARGB(_Effect.Alpha, 255, 255, 255);
		else
			_Color = D3DCOLOR_ARGB(int32_t(float((_Effect.MaxT - _Effect.T) / _Effect.MaxT) * _Effect.Alpha), 255, 255, 255);

		_Color = SwitchColorFromEffectID(_Effect.OBJ_ID, _Color);

		if (_Effect.bSmoke)
		{
			_Color.r = 0;
			_Color.g = 0;
			_Color.b = 0;
		};

		D3DXCOLOR RecordColor = _Color;

		_Color = SwitchColorFromGameState(_Effect.OBJ_ID,_Color);

		if (_Effect.bSmoke)
		{
			_Color.r = 0;
			_Color.g = 0;
			_Color.b = 0;
		}

		if ( (_Effect.bFlash && _Effect.bFlashRender )|| !_Effect.bFlash)
		{
			auto TexInfo = TextureManager::instance().Get_TexInfo(_Effect.ObjKey, _Effect.StateKey, _Effect.DrawID);
			RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width * _Effect.SrcScale.x, TexInfo->ImageInfo.Height   * _Effect.SrcScale.y  };
			vec3 Center = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
				&_srcRT, &Center, nullptr, _Color);


			if (_Effect.bRecord && global::IsPlay() && RecordManager::instance().bUpdate)
			{
				Record::Info _Info;
				_Info.SrcScale = _Effect.SrcScale;
				_Info.Alpha = 255;
				_Info.DrawID = _Effect.DrawID;
				_Info.MWorld = MWorld;
				_Info.ObjKey = _Effect.ObjKey;
				_Info.StateKey = _Effect.StateKey;
				_Info.Timing = Timing;
				_Info.OwnerY = _Effect.Pos.y;
				_Info._Color = RecordColor;
				_Record._Infos.insert({ Timing ,_Info });
			}
		}
	}

	DebugRender();
}
void EffectManager::DebugRender()
{
	if (!global::bDebug)return;

	static constexpr float DebugLineWidth = 2.f;

	std::pair<float, float > CameraPos{ 0.f,0.f };
	float JoomScale = global::JoomScale;

	matrix MJoom;
	CameraPos.first = global::CameraPos.x;
	CameraPos.second = global::CameraPos.y;

	GraphicDevice::instance().GetSprite()->End();
	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);

	MJoom = math::GetCameraJoomMatrix(JoomScale,
		vec3{ global::ClientSize.first,global::ClientSize.second,0.f });

	for (auto& _Effect : _Effects)
	{
		if (!_Effect.bPhysic)continue;
		matrix MWorld, MTrans, MRotZ, MScale;

		D3DXMatrixScaling(&MScale, _Effect.Scale.x, _Effect.Scale.y, _Effect.Scale.z);
		D3DXMatrixRotationZ(&MRotZ, 0/*-_Effect.RotZ*/);
		D3DXMatrixTranslation(&MTrans, _Effect.Pos.x, _Effect.Pos.y, _Effect.Pos.z);
		MWorld = MScale * MRotZ * MTrans;

		auto _EffectWorldRectPT = math::GetWorldRectPt(MWorld, _Effect.Width, _Effect.Height);
		std::array<vec2, 5ul> _RenderCollisionEft{};

		for (size_t i = 0; i < _EffectWorldRectPT.size(); ++i)
		{
			_RenderCollisionEft[i] = { _EffectWorldRectPT[i].x-
				 CameraPos.first   ,  _EffectWorldRectPT[i].y-CameraPos.second };
		};

		_RenderCollisionEft.back() = _RenderCollisionEft.front();
		GraphicDevice::instance().GetLine()->Draw(_RenderCollisionEft.data(),
		_RenderCollisionEft.size(), D3DCOLOR_ARGB(255,255,0,0));
	}

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
};

void EffectManager::Update()
{
	const float dt = Time::instance().Delta();

	for (size_t Idx = 0; Idx < _Effects.size(); ++Idx)
	{
		// �ִϸ��̼� ���� ���� ó��
		auto& _Effect = _Effects[Idx];
		_Effect.T += dt;
		_Effect.CurrentDelta += dt;

		// ������ ó��
		{
			if (_Effect.bFlash)
			{
				_Effect.FlashDt += dt;
				if (_Effect.FlashDt >= _Effect.FlashRepeat)
				{
					_Effect.FlashDt = 0.f;
					_Effect.bFlashRender = !_Effect.bFlashRender;
				}
			}
		}

		if (_Effect.CurrentDelta > _Effect.AnimDelta)
		{
			_Effect.CurrentDelta -= _Effect.AnimDelta;
			++_Effect.DrawID;

			if (_Effect.DrawID >= _Effect.End)
			{
				if (_Effect.bLoop)
				{
					_Effect.DrawID = 0;
				}
				else
				{
					_Effect.DrawID = _Effect.End-1;
				}
			}
		}

		// ������ ���� ����
		if (_Effect.bScaleLerp)
		{
			D3DXVec3Lerp(&_Effect.Scale, &_Effect.ScaleStart, &_Effect.ScaleGoal, _Effect.T / _Effect.MaxT);
		}

		D3DXVec2Lerp(&_Effect.SrcScale, &_Effect.SrcScaleStart, &_Effect.SrcScaleGoal, _Effect.T / _Effect.MaxT);

		if (_Effect.T > _Effect.MaxT)
		{
			_Effect = std::move(_Effects.back());
			_Effects.pop_back();
		}

		_Effect.RotZ += _Effect.RotZAcc * dt;
		_Effect.Pos += _Effect.Dir * dt;

		if (_Effect.bPhysic)
		{
			auto& _CompVec = ComponentManager::instance().Find<CollisionComponent>();

			for (auto& spCmp : _CompVec)
			{
				if (_EffectCollisionTagSet.contains(spCmp->_Tag))
				{
					if (!spCmp->bCollision)continue; 

					auto spOwner = spCmp->_Owner.lock();
					auto WorldRectPT = spCmp->GetWorldRectPt();
					vec3 OwnerPos = math::GetCenter(WorldRectPT);// spOwner->_TransformComp->Position;
					vec3 ToEffect = OwnerPos - _Effect.Pos;
					if (1000.f> D3DXVec3Length(&ToEffect))
					{
						matrix MWorld, MTrans, MRotZ, MScale;

						D3DXMatrixScaling(&MScale, _Effect.Scale.x, _Effect.Scale.y, _Effect.Scale.z);
						D3DXMatrixRotationZ(&MRotZ, 0/*-_Effect.RotZ*/);
						D3DXMatrixTranslation(&MTrans, _Effect.Pos.x, _Effect.Pos.y, _Effect.Pos.z);
						MWorld = MScale * MRotZ * MTrans;
					
						auto _EffectWorldRectPT = math::GetWorldRectPt(MWorld, _Effect.Width, _Effect.Height);

						auto opDir = math::Collision::RectAndRect({ WorldRectPT ,_EffectWorldRectPT }, false);

						if (opDir)
						{
							vec3 Normal = *opDir;

							if (std::abs(Normal.x) < std::abs(Normal.y))
								Normal.y = 0;
							else
								Normal.x = 0;

							/*vec3 PushDir;
							switch (_Effect.OBJ_ID)
							{
							case OBJECT_ID::EID::BULLET:

								break;
							default:
								break;
							}*/

							vec3 EffectCenter = math::GetCenter(_EffectWorldRectPT);
							vec3 CompCenter = math::GetCenter(WorldRectPT);
							vec3 _Dir = CompCenter - EffectCenter;
							D3DXVec3Normalize(&_Dir, &_Dir);
							math::Collision::HitInfo _HitInfo{};
							_HitInfo.PosDistance = D3DXVec3Length(&(*opDir));
							D3DXVec3Normalize(&(*opDir), &(*opDir));
							_HitInfo.CrossingAreaDir = *opDir;
							D3DXVec3Normalize(&Normal, &Normal);
							_HitInfo.Normal = Normal;
							_HitInfo.Position = _Effect.Pos;
							_HitInfo._Target = { };
							_HitInfo._ID = _Effect.OBJ_ID;
							_HitInfo._TAG = OBJECT_TAG::EEFFECT;
							_HitInfo.PosDir = _Dir;
							_HitInfo._Variable = std::ref(_Effect);

							spOwner->Hit({}, std::move(_HitInfo));
						}
					}
				}
		}
	}
};
}

void EffectManager::RecordRender()
{
	_Record.Render();
}

void EffectManager::Clear()
{
	_Effects.clear();

}

void EffectManager::RecordClear()
{
	_Record._Infos.clear();
}

void EffectManager::HitEvent(EffectInfo& _Effect, math::Collision::HitInfo _HitInfo)
{

}

D3DXCOLOR EffectManager::SwitchColorFromGameState(OBJECT_ID::EID _EffectID, D3DXCOLOR _Color)
{
	if (global::IsSlow())
	{
		_Color.g *= 0.3f;
		_Color.b *= 0.3f;
		_Color.r *= 0.3f;
	}
	else if (global::IsReplay())
	{
		_Color.g *= 0.3f;
		_Color.b *= 0.3f;
		_Color.r *= 0.3f;
	}

	switch (_EffectID)
	{
	case OBJECT_ID::REFLECT:
		_Color.r = 255;
		_Color.b = 255;
		_Color.g = 255;
		break;
	case OBJECT_ID::WHITE_BLOOD:
		_Color.r = 255;
		_Color.b = 0;
		_Color.g = 0;
		break;
	case OBJECT_ID::SLASH_FX:
		_Color.r = 255;
		_Color.g = 255;
		_Color.b = 255;
		break;
	case OBJECT_ID::BOSS_DASH:
		_Color.r = 255;
		_Color.b = 0;
		_Color.g = 0;
		break;
	case OBJECT_ID::LASER_DEAD:
		_Color.r = 255;
		_Color.g = 255;
		_Color.b = 0;
		break;
	case OBJECT_ID::BOSS_JUMP:
		_Color.r = 255;
		_Color.g = 0;
		_Color.b = 255;
		break;
	case OBJECT_ID::EXPLOSION:
		break;
	case OBJECT_ID::ITEM:
		break;
	case OBJECT_ID::GO:
		break;
	case OBJECT_ID::GANGSTER_GUN:
		break;
	case OBJECT_ID::REFLECT_BULLET:
		break;
	case OBJECT_ID::BULLET:
		break;
	case OBJECT_ID::HIT_EFFECT:
		break;
	case OBJECT_ID::GANGSTER:
		break;
	case OBJECT_ID::HIT_IMPACT:
		break;
	case OBJECT_ID::STOMPER_CLOUD:
		break;
	case OBJECT_ID::JUMP_CLOUD:
		break;
	case OBJECT_ID::LAND_CLOUD:
		break;
	case OBJECT_ID::GRUNT_SLASH:
		break;
	case OBJECT_ID::GRUNT:
		break;
	case OBJECT_ID::DustCloud:
		break;
	case OBJECT_ID::Effect:
		break;
	case OBJECT_ID::ATTACK:
		break;
	case OBJECT_ID::ATTACK_SLASH:
		break;
	case OBJECT_ID::ELINE:
		break;
	case OBJECT_ID::ETILE:
		break;
	case OBJECT_ID::EDOWNJUMPTILE:
		break;
	case OBJECT_ID::EWALLRIDELINE:
		break;
	case OBJECT_ID::EPLAYER:
		break;
	case OBJECT_ID::CHARACTER:
		break;
	case OBJECT_ID::CAMERA:
		break;
	case OBJECT_ID::UI_TIMER:
		break;
	case OBJECT_ID::UI_ITEMICONS:
		break;
	case OBJECT_ID::HUD:
		break;
	case OBJECT_ID::BATTERY:
		break;
	case OBJECT_ID::MOUSE:
		break;
	case OBJECT_ID::EENEMY1:
		break;
	case OBJECT_ID::EENEMY2:
		break;
	case OBJECT_ID::EENEMY3:
		break;
	case OBJECT_ID::ENONE:
		break;
	default:
		break;
	}
	return _Color;
}

D3DXCOLOR EffectManager::SwitchColorFromEffectID(OBJECT_ID::EID _EffectID,D3DXCOLOR _Color)
{
	switch (_EffectID)
	{
	case OBJECT_ID::SLASH_FX:
		_Color.r = 255;
		_Color.g = 255;
		_Color.b = 255;
		break;
	case OBJECT_ID::WHITE_BLOOD:
		_Color.r = 255;
		_Color.b = 0;
		_Color.g = 0;
		break;
	case OBJECT_ID::REFLECT:
		_Color.r = 255;
		_Color.b = 255;
		_Color.g = 255;
		break;
	case OBJECT_ID::DRAGON_DASH:
		_Color.r = 0;
		_Color.b = 235;
		_Color.g = 199;
		break;
	case OBJECT_ID::BOSS_DASH:
		_Color.r = 255;
		_Color.b = 0;
		_Color.g = 0;
		break;
	case OBJECT_ID::BOSS_JUMP:
		_Color.r = 255;
		_Color.g = 0;
		_Color.b = 255;
		break;
	case OBJECT_ID::LASER_DEAD:
		_Color.r = 255;
		_Color.g = 255;
		_Color.b = 0;
		break;
	case OBJECT_ID::GRUNT_SLASH:
		break;
	case OBJECT_ID::GRUNT:
		break;
	case OBJECT_ID::DustCloud:
		break;
	case OBJECT_ID::Effect:
		break;
	case OBJECT_ID::ATTACK:
		break;
	case OBJECT_ID::ATTACK_SLASH:
		break;
	case OBJECT_ID::ELINE:
		break;
	case OBJECT_ID::ETILE:
		break;
	case OBJECT_ID::EDOWNJUMPTILE:
		break;
	case OBJECT_ID::EWALLRIDELINE:
		break;
	case OBJECT_ID::EPLAYER:
		break;
	case OBJECT_ID::CHARACTER:
		break;
	case OBJECT_ID::CAMERA:
		break;
	case OBJECT_ID::UI_TIMER:
		break;
	case OBJECT_ID::UI_ITEMICONS:
		break;
	case OBJECT_ID::HUD:
		break;
	case OBJECT_ID::BATTERY:
		break;
	case OBJECT_ID::MOUSE:
		break;
	case OBJECT_ID::EENEMY1:
		break;
	case OBJECT_ID::EENEMY2:
		break;
	case OBJECT_ID::EENEMY3:
		break;
	case OBJECT_ID::ENONE:
		break;
	default:
		break;
	};

	return _Color;
}

D3DXCOLOR EffectManager::ConvertRecordColorFromEffectID(OBJECT_ID::EID _EffectID, D3DXCOLOR _Color)
{
	switch (_EffectID)
	{
	case OBJECT_ID::LASER_DEAD:
		break;
	case OBJECT_ID::LASER_TRAP:
		break;
	case OBJECT_ID::PANIC_SWITCH:
		break;
	case OBJECT_ID::BOSS_RECOVER_BOMB:
		break;
	case OBJECT_ID::GRENADE:
		break;
	case OBJECT_ID::BOSS_JUMP:
		break;
	case OBJECT_ID::BOSS_LASER:
		break;
	case OBJECT_ID::BOSS_DASH:
		break;
	case OBJECT_ID::BOSS:
		break;
	case OBJECT_ID::DOOR_KICK_IMPACT:
		break;
	case OBJECT_ID::DOOR:
		break;
	case OBJECT_ID::DRAGON_DASH:
		break;
	case OBJECT_ID::SMOKE_CLOUD:
		break;
	case OBJECT_ID::REFLECT:
		break;
	case OBJECT_ID::DRUM:
		break;
	case OBJECT_ID::EXPLOSION:
		break;
	case OBJECT_ID::ITEM:
		break;
	case OBJECT_ID::GO:
		break;
	case OBJECT_ID::GANGSTER_GUN:
		break;
	case OBJECT_ID::REFLECT_BULLET:
		break;
	case OBJECT_ID::BULLET:
		break;
	case OBJECT_ID::HIT_EFFECT:
		break;
	case OBJECT_ID::GANGSTER:
		break;
	case OBJECT_ID::HIT_IMPACT:
		break;
	case OBJECT_ID::STOMPER_CLOUD:
		break;
	case OBJECT_ID::JUMP_CLOUD:
		break;
	case OBJECT_ID::LAND_CLOUD:
		break;
	case OBJECT_ID::SLASH_FX:
		break;
	case OBJECT_ID::WHITE_BLOOD:
		_Color.r = 255;
		_Color.g = 255;
		_Color.b = 255;
		break;
	case OBJECT_ID::GRUNT_SLASH:
		break;
	case OBJECT_ID::GRUNT:
		break;
	case OBJECT_ID::DustCloud:
		break;
	case OBJECT_ID::Effect:
		break;
	case OBJECT_ID::ATTACK:
		break;
	case OBJECT_ID::ATTACK_SLASH:
		break;
	case OBJECT_ID::ELINE:
		break;
	case OBJECT_ID::ETILE:
		break;
	case OBJECT_ID::EDOWNJUMPTILE:
		break;
	case OBJECT_ID::EWALLRIDELINE:
		break;
	case OBJECT_ID::EPLAYER:
		break;
	case OBJECT_ID::CHARACTER:
		break;
	case OBJECT_ID::CAMERA:
		break;
	case OBJECT_ID::UI_TIMER:
		break;
	case OBJECT_ID::UI_ITEMICONS:
		break;
	case OBJECT_ID::HUD:
		break;
	case OBJECT_ID::BATTERY:
		break;
	case OBJECT_ID::MOUSE:
		break;
	case OBJECT_ID::EENEMY1:
		break;
	case OBJECT_ID::EENEMY2:
		break;
	case OBJECT_ID::EENEMY3:
		break;
	case OBJECT_ID::ENONE:
		break;
	default:
		break;
	}
	return _Color;
}

void EffectManager::EffectPush
(const std::wstring_view& _ObjKey, 
	const std::wstring_view& _StateKey, 
	uint8_t _End, float AnimDelta, 
	float MaxT, OBJECT_ID::EID _EffectID, bool bRecord,
	vec3 Pos, vec3 Dir, vec3 Scale, 
	bool bLoop, bool bPhysic, 
	bool bMapSlide, bool bHitNotify, 
	float Width, float Height, 
	int32_t Alpha ,
	bool bAlphaLerp , 
	float T,
	float RotZ, float RotZAcc, uint8_t _StartID,
	bool bFlash,
	float FlashRepeat,
	int32_t Layer,
	bool bScaleLerp ,
	vec3 ScaleStart,
	vec3 ScaleGoal,
	vec2 SrcScaleStart,
	vec2 SrcScaleGoal
	)
{
	EffectInfo _Info;
	_Info.ObjKey = _ObjKey;
	_Info.StateKey = _StateKey;
	_Info.End = _End;
	_Info.AnimDelta = AnimDelta;
	_Info.MaxT = MaxT;
	_Info.OBJ_ID = _EffectID;
	_Info.Pos = Pos;
	_Info.Dir = Dir;
	_Info.Scale = Scale;
	_Info.bLoop = bLoop;
	_Info.bPhysic = bPhysic;
	_Info.bMapSlide = bMapSlide;
	_Info.bHitNotify = bHitNotify;
	_Info.Width = Width;
	_Info.Height = Height;
	_Info.T = T;
	_Info.RotZ = RotZ;
	_Info.RotZAcc = RotZAcc;
	_Info.DrawID = _StartID;
	_Info.Alpha = Alpha; 
	_Info.bAlphaLerp = bAlphaLerp;
	_Info.bRecord = bRecord;
	_Info.bFlash = bFlash;
	_Info.FlashRepeat=FlashRepeat; 
	_Info.Layer=Layer;
	_Info.bScaleLerp = bScaleLerp;
	_Info.SrcScaleGoal = SrcScaleGoal;
	_Info.SrcScaleStart = SrcScaleStart;
	if (bScaleLerp)
	{
		_Info.ScaleGoal = ScaleGoal;
		_Info.ScaleStart = ScaleStart;
	}
	else
	{
		_Info.ScaleGoal = _Info.Scale;
		_Info.ScaleStart = _Info.Scale;
	}
	
	_Effects.emplace_back(std::move(_Info));
}

