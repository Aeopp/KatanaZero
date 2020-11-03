#include "stdafx.h"

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

void NormalEnemy::Initialize() & noexcept
{
	Super::Initialize();

	_FollowRenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);
	_FollowRenderComp->bRender = false;
	_FollowRenderComp->AfterImgOff();
	_FollowRenderComp->PositionCorrection = vec3{ 0.f,-60.f,0.f };
	_FollowRenderComp->Anim(true, false, L"follow", 2, 1.2f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"spr_enemy_follow", LAYER::ELAYER::EOBJECT);
	
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
		constexpr uint32_t CirclePointCount = 18;
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
	if (!_Target.expired() && _Target.lock()->GetID() == OBJECT_ID::ATTACK_SLASH)
	{
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

		_RenderComp->AfterImgOn();
		_EnemyState = NormalEnemy::State::Die;

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce*5, _CollisionInfo.PushDir, 0.3f);

			Time::instance().TimeScale = 0.2f;
			Time::instance().TimerRegist(0.07f, 0.07f, 0.07f, [this]() {
				
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

bool NormalEnemy::IsSamefloor(vec3 TargetPos)
{
	float y = _PhysicComp->Position.y;

	float upper = y + IsSamefloorRange.second;
	float lower = y + IsSamefloorRange.first;

	if (TargetPos.y > lower && TargetPos.y < upper)
	{
		return true;
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
		if (IsSamefloor(TargetLocation))
		{
			// 감지범위 이내에 존재하고 바라보는 방향에 있다면 성공
			IsRangeInner |= ToTargetDistance < _DetectionRange && (Dot > 0.f);
			// 플레이어가 살금살금 움직이는 중이 아니라면 좀더 좁은범위에 존재한다면 방향과 상관없이 성공
			if (!bIsPlayerSneak)
				IsRangeInner |= ToTargetDistance < _NarrowRange;
			// 여기서 Y Range 를 조사. 같은 층에 있는지 검출.
		}
		break;
	case NormalEnemy::State::Detecting:
		// Detect 상태일때 감지범위 이내에만 있다면 성공
		IsRangeInner = ToTargetDistance < PursuitRange;
		break;
	default:
		break;
	}
	
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
			_FollowRenderComp->bRender = true;
			_FollowRenderComp->Anim(true, false, L"follow", 2, 1.2f);
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

