#include "stdafx.h"

#include "ObjectManager.h"
#include "Time.h"
#include "NormalEnemy.h"
#include "Player.h"
#include "CollisionComponent.h"
#include "RenderComponent.h"
#include "GraphicDevice.h"
#include "PhysicTransformComponent.h"

void NormalEnemy::Initialize() & noexcept
{
	Super::Initialize();

	_RenderComp->_RenderAfterEvent = [this]() 
	{	
		if (!global::bDebug)return;

		float Radius = DetectionRange;
		switch (_CurState)
		{
		case NormalEnemy::State::Detecting:
			Radius = PursuitRange;
			break;
		case NormalEnemy::State::Idle:
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
}

void NormalEnemy::LateUpdate()
{
	Super::LateUpdate();
	const float Dt = Time::instance().Delta(); 
	_DetectionRangeColor = math::lerp(_DetectionRangeColor, _DetectionRangeColorGoal, ColorGoalTime, Dt);

	// 추격중인데 플레이어가 아래에 존재한다면 라인을 타기위해 아래점프가 가능한 벽과 충돌을 해제
	if (_CurState ==NormalEnemy::State::Detecting)
	{
		if (ToTarget.y > 5.f)
		{
			_CollisionComp->bDownJump = true;
		}
	}
	else
	{
		_CollisionComp->bDownJump = false;
	}
}

void NormalEnemy::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target, _CollisionInfo);

	if (_CurState == NormalEnemy::State::Die)
	{
		return;
	}
	if (_Target.lock()->GetID() == OBJECT_ID::ATTACK_SLASH)
	{
		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce *3.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		_CurState = NormalEnemy::State::Die;

		Die();
	}
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
	
	switch (_CurState)
	{
	case NormalEnemy::State::Idle:
		break;
	case NormalEnemy::State::Detecting:
		if (IsTurn(ToTargetDir))
		{
			_PhysicComp->Dir = ConvertMoveDir(ToTargetDir);
			Turn();
		}
		else
			_PhysicComp->Dir = ConvertMoveDir(ToTargetDir);
		break;
	default:
		break;
	}

	bool IsRangeInner = false;
	bool bIsPlayerSneak = ObjectManager::instance()._Player.lock()->bSneak;
	ObjectManager::instance()._Player.lock()->bSneak;

	switch (_CurState)
	{
	case NormalEnemy::State::Idle:
		// 감지범위 이내에 존재하고 바라보는 방향에 있다면 성공
		IsRangeInner |= ToTargetDistance < DetectionRange && (Dot > 0);
		// 플레이어가 살금살금 움직이는 중이 아니라면 좀더 좁은범위에 존재한다면 방향과 상관없이 성공
		if(!bIsPlayerSneak)
			IsRangeInner |= ToTargetDistance < NarrowRange;
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
		switch (_CurState)
		{
		case NormalEnemy::State::Detecting:
			break;
		case NormalEnemy::State::Idle:
			_DetectionRangeColorGoal = D3DCOLOR_ARGB(255, 255, 0, 0);
			_CurState = NormalEnemy::State::Detecting;
			break;
		default:
			break;
		}

		return true;
	}
	else
	{
		switch (_CurState)
		{
		case NormalEnemy::State::Detecting:
			_DetectionRangeColorGoal = D3DCOLOR_ARGB(255, 0, 255, 0);
			_CurState = NormalEnemy::State::Idle;
			break;
		case NormalEnemy::State::Idle:
			break;
		default:
			break;
		}
	}

	return false;
};


bool NormalEnemy::IsTurn(vec3 Dir)
{
	return  ((_PhysicComp->Dir.x * Dir.x) < 0.f);
}

vec3 NormalEnemy::ConvertMoveDir(vec3 Dir)
{
	D3DXVec3Normalize(&Dir, &Dir);

	if (Dir.x < 0.f)
		Dir = vec3{ -1.f,0.f,0.f };
	else
		Dir = vec3{ +1.f,0.f,0.f };

	return Dir;
}

