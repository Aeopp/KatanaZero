#include "stdafx.h"


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

void NormalEnemy::Initialize() & noexcept
{
	Super::Initialize();

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

	_RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 255, 0, 255);
	_RenderComp->SlowColor = D3DCOLOR_ARGB(255, 255, 0, 255);
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
		if (ToTarget.y > 15.f)
		{
			_CollisionComp->bDownJump = true;
		}
		else
		{
			_CollisionComp->bDownJump = false;
		}
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
	if (_Target.lock()->GetID() == OBJECT_ID::ATTACK_SLASH)
	{
		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce *3.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		_RenderComp->AfterImgOn();
		_EnemyState = NormalEnemy::State::Die;

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce*5, _CollisionInfo.PushDir, 0.3f);

			Time::instance().TimeScale = 0.2f;
			Time::instance().TimerRegist(0.07f, 0.07f, 0.07f, [this]() {
				
				if (global::ECurGameState::Slow != global::_CurGameState)
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
	  	_PhysicComp->Dir = ConvertMoveDir(ToTargetDir);
		break;
	default:
		break;
	}

	bool IsRangeInner = false;
	bool bIsPlayerSneak = ObjectManager::instance()._Player.lock()->bSneak;
	ObjectManager::instance()._Player.lock()->bSneak;

	// ������ �������¶�� �������� ����
	float _DetectionRange = bLaziness ?  (DetectionRange*0.5f): DetectionRange;
	float _NarrowRange = bLaziness ? (NarrowRange*1.f) : NarrowRange;

	switch (_EnemyState)
	{
	case NormalEnemy::State::Idle:
	case NormalEnemy::State::Walk:
		if (IsSamefloor(TargetLocation))
		{
			// �������� �̳��� �����ϰ� �ٶ󺸴� ���⿡ �ִٸ� ����
			IsRangeInner |= ToTargetDistance < _DetectionRange && (Dot > 0.f);
			// �÷��̾ ��ݻ�� �����̴� ���� �ƴ϶�� ���� ���������� �����Ѵٸ� ����� ������� ����
			if (!bIsPlayerSneak)
				IsRangeInner |= ToTargetDistance < _NarrowRange;
			// ���⼭ Y Range �� ����. ���� ���� �ִ��� ����.
		}
		break;
	case NormalEnemy::State::Detecting:
		// Detect �����϶� �������� �̳����� �ִٸ� ����
		IsRangeInner = ToTargetDistance < PursuitRange;
		break;
	default:
		break;
	}
	
	if (IsRangeInner)
	{
		// ������ �����Ͽ����� ���� ��ȯ
		switch (_EnemyState)
		{
		case NormalEnemy::State::Detecting:
			break;
		case NormalEnemy::State::Idle:
		case NormalEnemy::State::Walk:
			_DetectionRangeColorGoal = D3DCOLOR_ARGB(255, 255, 0, 0);
			_EnemyState = NormalEnemy::State::Detecting;
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

vec3 NormalEnemy::ConvertMoveDir(vec3 Dir)
{
	D3DXVec3Normalize(&Dir, &Dir);

	if (Dir.x < 0.f)
		Dir = vec3{ -1.f,0.f,0.f };
	else
		Dir = vec3{ +1.f,0.f,0.f };

	return Dir;
}

