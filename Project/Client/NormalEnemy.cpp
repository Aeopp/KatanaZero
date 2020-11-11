#include "stdafx.h"

#include "Item.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "Time.h"
#include "NormalEnemy.h"
#include "Player.h"
#include "CollisionComponent.h"
#include "RenderComponent.h"
#include "GraphicDevice.h"
#include "PhysicTransformComponent.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "ComponentManager.h"
#include "Door.h"
#include "sound_mgr.h"


void NormalEnemy::Initialize() & noexcept
{
	Super::Initialize();

	_MsgRenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
	_MsgRenderComp->bSmokeAffected = false;
	_MsgRenderComp->bRender = false;
	_MsgRenderComp->AfterImgOff();
	_MsgRenderComp->PositionCorrection = vec3{ 0.f,-60.f,0.f };
	//_MsgRenderComp->Anim(true, false, L"follow", 2, 1.2f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"spr_enemy_follow", LAYER::ELAYER::EOBJECT);

	_RenderComp->_RenderAfterEvent = [this]() 
	{	
		if (!global::bDebug)return;

		float Radius = DetectionRange;
		switch (_EnemyState)
		{
		case NormalEnemy::State::Detecting:
			Radius = PursuitRange;
			break;
		case NormalEnemy::State::Idle:
		case NormalEnemy::State::Walk:
			Radius = DetectionRange;
			break;
		case NormalEnemy::State::Die:
			return;
			break;
		default:
			break;
		}

		constexpr float DetectionRangeLineWidth = 2.f;
		constexpr uint32_t CirclePointCount = 36;
		constexpr float _2PI = math::PI;
		constexpr float PointBetWeenAngle = _2PI / (float)CirclePointCount;

		vec3 Center = _PhysicComp->Position;

		std::array<vec2, CirclePointCount * 2 + 2> _PointArr;

		for (uint32_t i = 0; i < CirclePointCount*2+2; ++i)
		{
				vec3 Pos = (Center + vec3{ cosf(PointBetWeenAngle * i) * Radius,
							sinf(PointBetWeenAngle * i) * Radius ,0.f});
				_PointArr[i].x = Pos.x;
				_PointArr[i].y = Pos.y;
				_PointArr[i].x -= global::CameraPos.x;
				_PointArr[i].y -= global::CameraPos.y;
				D3DXVec2TransformCoord(&_PointArr[i], &_PointArr[i],
					&math::GetCameraJoomMatrix(global::JoomScale,
						vec3{ global::ClientSize.first,global::ClientSize.second,0.f }));
		}

		GraphicDevice::instance().GetSprite()->End();
		GraphicDevice::instance().GetLine()->SetWidth(DetectionRangeLineWidth);
		GraphicDevice::instance().GetLine()->Draw(_PointArr.data(), _PointArr.size(),_DetectionRangeColor);
		GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
	};

	_DetectionRangeColorGoal = D3DCOLOR_ARGB(255, 0, 255, 0);

	_RenderComp->SlowDeltaCoefft = 0.05f;
	_RenderComp->AfterDeltaCoefft = 0.07f;

	_RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	_RenderComp->SlowColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	_RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 0);
}

void NormalEnemy::LateUpdate()
{
	Super::LateUpdate();
	const float Dt = Time::instance().Delta(); 
	_DetectionRangeColor = math::lerp(_DetectionRangeColor, _DetectionRangeColorGoal, _DetectionRangeColorGoalTime, Dt);

	switch (_EnemyState)
	{
	case NormalEnemy::State::Idle:
	case NormalEnemy::State::Walk:
		break;
	case NormalEnemy::State::Detecting:
		/*if (ToTarget.y > 15.f)
		{
			_CollisionComp->bDownJump = true;
		}
		else
		{
			_CollisionComp->bDownJump = false;
		}*/
		break;
	case NormalEnemy::State::Die:
		break;
	default:
		break;
	}
}


void NormalEnemy::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target, _CollisionInfo);

	if (_EnemyState == NormalEnemy::State::Die)
	{
		return;
	}

	if (_CollisionInfo._ID == OBJECT_ID::LASER_TRAP)
	{

		ObjectManager::instance()._Camera.lock()->CameraShake(
			500.f, math::Rand<int32_t>({ 0,1 }) ? vec3{ 1,0,0 } : vec3{ -1,0,0 }, 0.7);


		RAND_SOUNDPLAY("death_generic", { 1,3 }, 0.6f, false);

		_MsgRenderComp->bRender = false;
		_EnemyState = NormalEnemy::State::Die;
		_RenderComp->bRender = false;
		vec3 Scale = _TransformComp->Scale;
		Scale.x *= _RenderComp->AnimDir;

		EffectManager::instance().EffectPush(_RenderComp->_Info.ObjectKey,
			_RenderComp->_Info.StateKey, _RenderComp->_Info.GetCurFrame() + 1, 2.f, 2.f, OBJECT_ID::EID::LASER_DEAD,
			true, _TransformComp->Position, { 0,0,0 }, Scale, false, false, false, false, 0, 0, 255, true, 0, 0, 0, _RenderComp->_Info.GetCurFrame(), false,
			0, 0, false, Scale, Scale, { 1,1 }, { 1,0 });

		for (size_t i = 0; i < 7; ++i)
		{
			Time::instance().TimerRegist(0.75f, (std::numeric_limits<float>::min)(), 2.f, [this, Observer = _This, SparkParticleLocation = _TransformComp->Position]() {
				if (Observer.expired())return true;
				vec3 ScaleStart = { math::Rand<float>({10,20}),math::Rand<float>({10,20}),1 };

				EffectManager::instance().EffectPush(L"Effect", L"spr_spark_particle", 1, 1.f, 2.f, OBJECT_ID::EID::ENONE,
					true, SparkParticleLocation, math::RotationVec({ 1,0,0 }, math::Rand<float>({ -360,360 }))* math::Rand<float>({ 100,300 }), ScaleStart,
					true, false, false, false, 0, 0, 255, true, 0, 0, 0, 0, false, false, 0, true, ScaleStart, { 0,0,0 }, { 1,1 }, { 1,1 });

				return false; });
		}
	}

	if (_CollisionInfo._ID == OBJECT_ID::DOOR_KICK_IMPACT)
	{
			vec3 PushDir = { 1,0,0 };
			if (_CollisionInfo.PosDir.x<0.f)
			{
				PushDir.x = -1.f;
			}
			_CollisionInfo.PushDir = PushDir;
			_CollisionInfo.PushForce = 100.f;

			_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 3.5f),
				500.f,
				0.6f,
				_CollisionInfo.PushDir);

			BloodInit(_CollisionInfo.PushDir);

			float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);

			EffectManager::instance().EffectPush(L"Effect",
				L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
				_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
				{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
				255, false, 0, ImpactRotZ, 0, 0);

			vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

			EffectManager::instance().EffectPush(L"Effect",
				L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
				_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
				, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
				0, 0);

			_MsgRenderComp->bRender = false;

			_RenderComp->AfterImgOn();
			_EnemyState = NormalEnemy::State::Die;

			ObjectManager::instance()._Camera.lock()->CameraShake(
				_CollisionInfo.PushForce * 3.5f, _CollisionInfo.PushDir, 0.3f);

			Time::instance().TimeScale = 0.2f;
			Time::instance().TimerRegist(0.04f, 0.04f, 0.04f, [this]() {

				if (global::ECurGameState::PlaySlow != global::_CurGameState)
				{
					Time::instance().TimeScale = 1.f;
				}
				return true; });

			_RenderComp->AfterImgOn();
			_RenderComp->NormalAfterImgPushDelta *= 1.f;

			Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]()
			{
				_RenderComp->AfterImgOff();
				_RenderComp->NormalAfterImgPushDelta *= 1.f;
				return true;
			});

			Die();
	}

	if (_CollisionInfo._ID == OBJECT_ID::EID::DRAGON_DASH)
	{
		_CollisionInfo.PushDir = _CollisionInfo.PosDir;
		D3DXVec3Normalize(&_CollisionInfo.PushDir, &_CollisionInfo.PushDir);

		_CollisionInfo.PushForce = 500.f;

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 3.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		BloodInit(_CollisionInfo.PushDir);

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_slashfx", 5, 0.02f,
			0.02f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false, _PhysicComp->Position,
			{ 0,0,0 }, { 2.9,2.9,0 });

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

		EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);

		_MsgRenderComp->bRender = false;

		_RenderComp->AfterImgOn();
		_EnemyState = NormalEnemy::State::Die;

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce * 2, _CollisionInfo.PushDir, 0.1f);

		Time::instance().TimeScale = 0.2f;
		Time::instance().TimerRegist(0.05f, 0.05f, 0.05f, [this]() {

			if (global::ECurGameState::PlaySlow != global::_CurGameState)
			{
				Time::instance().TimeScale = 1.f;
			}
			return true; });

		_RenderComp->AfterImgOn();
		_RenderComp->NormalAfterImgPushDelta *= 1.f;

		Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]()
		{
			_RenderComp->AfterImgOff();
			_RenderComp->NormalAfterImgPushDelta *= 1.f;
			return true;
		});
		SwordDeathSound();

		Die();
	}

	if (_CollisionInfo._ID==OBJECT_ID::EID::EXPLOSION)
	{
		_CollisionInfo.PushDir = { 0,-1,0 };
		_CollisionInfo.PushDir += _CollisionInfo.PosDir;
		D3DXVec3Normalize(&_CollisionInfo.PushDir, &_CollisionInfo.PushDir);

		_CollisionInfo.PushForce = 500.f;

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 3.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

		/*EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);*/

		_MsgRenderComp->bRender = false;

		_RenderComp->AfterImgOn();
		 _EnemyState = NormalEnemy::State::Die;

		ObjectManager::instance()._Camera.lock()->CameraShake(
			2500.f, _CollisionInfo.PushDir, 0.2f);

		Time::instance().TimeScale = 0.2f;
		Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]() {

			if (global::ECurGameState::PlaySlow != global::_CurGameState)
			{
				Time::instance().TimeScale = 1.f;
			}
			return true; });

		_RenderComp->AfterImgOn();
		_RenderComp->NormalAfterImgPushDelta *= 1.f;

		Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]()
		{
			_RenderComp->AfterImgOff();
			_RenderComp->NormalAfterImgPushDelta *= 1.f;
			return true;
		});

		Die();
		return;
	}
	else if (_CollisionInfo._ID == OBJECT_ID::ITEM)
	{
		auto _Item = std::dynamic_pointer_cast<Item>(_CollisionInfo._Target.lock());

		if (_Item->_ItemID == EItem::Knife && _Item->bThrow)
		{
			_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 3.5f),
				_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
				0.3f,
				_CollisionInfo.PushDir);

			float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
			EffectManager::instance().EffectPush(L"Effect",
				L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
				_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
				{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
				255, false, 0, ImpactRotZ, 0, 0);

			vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

			EffectManager::instance().EffectPush(L"Effect",
				L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
				_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
				, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
				0, 0);

			_MsgRenderComp->bRender = false;
			SOUNDPLAY("blunt", 0.9f);

			SOUNDPLAY("death_knife", 0.8f);

			_RenderComp->AfterImgOn();
			//_EnemyState = NormalEnemy::State::Die;

			ObjectManager::instance()._Camera.lock()->CameraShake(
				1777.f, _CollisionInfo.PushDir, 0.20f);

			Time::instance().TimeScale = 0.2f;
			Time::instance().TimerRegist(0.05f, 0.05f, 0.05f, [this]() {

				if (global::ECurGameState::PlaySlow != global::_CurGameState)
				{
					Time::instance().TimeScale = 1.f;
				}
				return true; });

			_RenderComp->AfterImgOn();
			_RenderComp->NormalAfterImgPushDelta *= 1.f;

			Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]()
			{
				_RenderComp->AfterImgOff();
				_RenderComp->NormalAfterImgPushDelta *= 1.f;
				return true;
			});

			Die();
		}
		return;
	}
	else if (!_Target.expired() && _Target.lock()->GetID() == 
		OBJECT_ID::ATTACK_SLASH||   _CollisionInfo._ID == OBJECT_ID::REFLECT_BULLET)
	{
		if (_CollisionInfo._ID == OBJECT_ID::REFLECT_BULLET)
		{
			auto _RefEftInfo = _CollisionInfo._Variable._Cast<std::reference_wrapper<EffectInfo>>();
			_RefEftInfo->get().T = 0.f;
			_RefEftInfo->get().MaxT = 5.f;
			_RefEftInfo->get().bPhysic = false;
			_RefEftInfo->get().Scale.x *= 4.f;
			SOUNDPLAY("death_bullet", 0.9f);
		}
		else
		{
			SwordDeathSound();
		}

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce *3.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		BloodInit(_CollisionInfo.PushDir);

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_slashfx", 5, 0.02f,
			0.02f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false , _PhysicComp->Position,
			{ 0,0,0 }, { 2.9,2.9,0 });

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false ,
			_PhysicComp->Position + -_CollisionInfo.PushDir*77, 
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir*4000);

		EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false , Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);

		_MsgRenderComp->bRender = false;
		
		_RenderComp->AfterImgOn();
		_EnemyState = NormalEnemy::State::Die;

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce*3, _CollisionInfo.PushDir, 0.2f);

			Time::instance().TimeScale = 0.2f;
			Time::instance().TimerRegist(0.05f, 0.05f, 0.05f, [this]() {
				
				if (global::ECurGameState::PlaySlow != global::_CurGameState)
				{
					Time::instance().TimeScale = 1.f;
				}
				return true; });

			_RenderComp->AfterImgOn();
			_RenderComp->NormalAfterImgPushDelta *= 1.f;

			Time::instance().TimerRegist(0.1f, 0.1f, 0.1f, [this]()
			{
				_RenderComp->AfterImgOff();
				_RenderComp->NormalAfterImgPushDelta *= 1.f;
				return true;
			});

		Die();
	}
}

bool NormalEnemy::IsAlive() const&
{
		return 	_EnemyState != NormalEnemy::State::Die;
}

void NormalEnemy::SwordDeathSound()
{
	RAND_SOUNDPLAY("death_sword", { 1,2 }, 0.6f, false);

	RAND_SOUNDPLAY("death_generic", { 1,3 }, 0.6f, false);
}

bool NormalEnemy::IsSamefloor(vec3 TargetPos,bool bDoor)
{
	float y = _PhysicComp->Position.y;

	float upper = y + IsSamefloorRange.second;
	float lower = y + IsSamefloorRange.first;

	if (bDoor)
	{
		lower -= 80; 
		upper += 80;
	}

	if (TargetPos.y > lower && TargetPos.y < upper)
	{
		return true;
	}

	return false;
}

bool NormalEnemy::IsDetectDoorCheck()
{
	auto& DoorVec = ObjectManager::instance()._Doors;
	for (auto& wpDoor : DoorVec)
	{
		auto spDoor = std::dynamic_pointer_cast<Door>(wpDoor.lock());
		if ( spDoor&&!spDoor->bOpen)
		{
			if (IsSamefloor(spDoor->_TransformComp->Position,true))
			{
				vec3 ToTarget = _Target->_TransformComp->Position - _TransformComp->Position;
				vec3 DoorLocationCorrection = spDoor->_TransformComp->Position;
				DoorLocationCorrection.x += Door::XCorrection;
				vec3 ToDoor = DoorLocationCorrection - _TransformComp->Position;
				float ToTargetLength = D3DXVec3Length(&ToTarget);
				float ToDoorLength = D3DXVec3Length(&ToDoor);

				if (D3DXVec3Dot(&ToTarget, &ToDoor) > 0.f)
				{
					if (ToTargetLength > ToDoorLength)
					{
						return true;
					}									
				}
			}
		}
	}

	return false;
}

bool NormalEnemy::IsRangeInnerTarget()
{

	vec3 TargetLocation = _Target->_PhysicComp->Position;
	vec3 MyLocation = _PhysicComp->Position;

	ToTarget = TargetLocation - MyLocation;
	float Distance = D3DXVec3Length(&ToTarget);

	vec3 Dir;
	D3DXVec3Normalize(&Dir, &ToTarget);
	ToTargetDir = Dir;
	ToTargetDistance = Distance;

	float Dot = D3DXVec3Dot(&_PhysicComp->Dir, &ToTargetDir);
	
	switch (_EnemyState)
	{
	case NormalEnemy::State::Idle:
	case NormalEnemy::State::Walk:
		break;
	case NormalEnemy::State::Detecting:
	  	_PhysicComp->Dir = ConvertXAxisDir(ToTargetDir);
		break;
	default:
		break;
	}

	bool IsRangeInner = false;
	bool bIsPlayerSneak = ObjectManager::instance()._Player.lock()->bSneak;
	ObjectManager::instance()._Player.lock()->bSneak;

	// 게으른 순찰상태라면 감지범위 감소
	float _DetectionRange = bLaziness ?  (DetectionRange*0.5f): DetectionRange;
	float _NarrowRange = bLaziness ? (NarrowRange*1.f) : NarrowRange;

	switch (_EnemyState)
	{
	case NormalEnemy::State::Idle:
	case NormalEnemy::State::Walk:
		if (IsSamefloor(TargetLocation,false ))
		{
			// 감지범위 이내에 존재하고 바라보는 방향에 있다면 성공
			IsRangeInner |= ToTargetDistance < _DetectionRange && (Dot > 0.f);
			// 플레이어가 살금살금 움직이는 중이 아니라면 좀더 좁은범위에 존재한다면 방향과 상관없이 성공
			if (!bIsPlayerSneak)
				IsRangeInner |= ToTargetDistance < _NarrowRange;
			// 여기서 Y Range 를 조사. 같은 층에 있는지 검출.

			// 닫힌 문너머에 있는지를 조사 같은 층에있는 닫힌 문너머에  있다면 실패
			if (IsDetectDoorCheck())
			{
				IsRangeInner = false;
			}
		}
		break;
	case NormalEnemy::State::Detecting:
		// Detect 상태일때 감지범위 이내에만 있다면 성공
		IsRangeInner = ToTargetDistance < PursuitRange;
		break;
	default:
		break;
	}
	RenderComponent::NotifyType _Notify;
	
	if (IsRangeInner)
	{
		// 감지가 성공하였으니 상태 전환
		switch (_EnemyState)
		{
		case NormalEnemy::State::Detecting:
			break;
		case NormalEnemy::State::Idle:
		case NormalEnemy::State::Walk:
			_DetectionRangeColorGoal = D3DCOLOR_ARGB(255, 255, 0, 0);
			_EnemyState = NormalEnemy::State::Detecting;
			_MsgRenderComp->bRender = true;

			_Notify[2] = [this]() 
			{
				_MsgRenderComp->Anim(true, false, L"follow", 2, 1, {},
					D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"spr_enemy_follow",
					LAYER::ELAYER::EOBJECT_OVER);
			};

			_MsgRenderComp->Anim(true, false, L"exclaim", 2, 0.5f, std::move(_Notify),
				D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"spr_enemy_exclaim",
				LAYER::ELAYER::EOBJECT_OVER);
			break;
		default:
			break;
		}
		return true;
	}
	else
	{
		switch (_EnemyState)
		{
		case NormalEnemy::State::Detecting:
			_DetectionRangeColorGoal = D3DCOLOR_ARGB(255, 0, 255, 0);
			_EnemyState = NormalEnemy::State::Idle;
			break;
		case NormalEnemy::State::Idle:
			break;
		case NormalEnemy::State::Walk:
			break;
		default:
			break;
		}
	}

	return false;
};

vec3 NormalEnemy::ConvertXAxisDir(vec3 Dir)
{
	D3DXVec3Normalize(&Dir, &Dir);

	if (Dir.x < 0.f)
		Dir = vec3{ -1.f,0.f,0.f };
	else
		Dir = vec3{ +1.f,0.f,0.f };

	return Dir;
}

