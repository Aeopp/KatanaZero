#include "stdafx.h"
#include "Gangster.h"

#include "RenderComponent.h"
#include "EffectManager.h"
#include "Player.h"
#include "AStarManager.h"
#include "Grunt_Slash.h"
#include "CollisionComponent.h"
#include "PhysicTransformComponent.h"
#include "RenderComponent.h"
#include "ComponentManager.h"
#include "ObjectManager.h"
#include "Time.h"
#include "GraphicDevice.h"
#include "Component.h"
#include "Texture_Manager.h"

using namespace std;
auto RunRenderMake = [](float& RotZ)
{
	auto GunRender = [&](Component& _Comp) {

		auto& _RenderComp = dynamic_cast<RenderComponent&>(_Comp);
		if (!_RenderComp.bRender)return;

		const float dt = Time::instance().Delta();

		_RenderComp.Component::Render();

		auto spOwner = _RenderComp._Owner.lock();
		auto spTexInfo = TextureManager::instance().Get_TexInfo
		(_RenderComp._Info.ObjectKey, _RenderComp._Info.StateKey, _RenderComp._Info.GetCurFrame());

		if (!spOwner)return;
		if (!spOwner->_TransformComp)return;
		if (!spTexInfo)return;

		const float JoomScale = global::JoomScale;
		vec3 CameraPos = global::CameraPos;

		matrix MWorld,MScale, MRotZ,MTrans  ; //  = spOwner->_TransformComp->CalcWorldMatrix(true);
		D3DXMatrixScaling(&MScale, spOwner->_TransformComp->Scale.x, spOwner->_TransformComp->Scale.y, spOwner->_TransformComp->Scale.z);
		D3DXMatrixRotationZ(&MRotZ, RotZ);
		D3DXMatrixTranslation(&MTrans, spOwner->_TransformComp->Position.x - global::CameraPos.x, spOwner->_TransformComp->Position.y - global::CameraPos.y, spOwner->_TransformComp->Position.z);

		MWorld = MScale * MRotZ * MTrans;

		MWorld = MWorld * math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });
		MWorld._11 *= _RenderComp.AnimDir;
		MWorld._41 += _RenderComp.PositionCorrection.x;
		MWorld._42 += _RenderComp.PositionCorrection.y;
		MWorld._43 += _RenderComp.PositionCorrection.z;

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
				for (auto _AfterIter = begin(_RenderComp._AfterImgVec);
					_AfterIter != end(_RenderComp._AfterImgVec);)
				{
					auto& _After = *_AfterIter;

					auto TexInfo = TextureManager::instance().
						Get_TexInfo(_RenderComp._Info.ObjectKey, _After.StateKey, _After.ID);
					RECT _srcRT = { 0,0,TexInfo->ImageInfo.Width * _RenderComp._Info.SrcScale.x,
								  TexInfo->ImageInfo.Height * _RenderComp._Info.SrcScale.y };
					vec3 __TextureCenter = { TexInfo->ImageInfo.Width / 2.f,TexInfo->ImageInfo.Height / 2.f,0.f };
					GraphicDevice::instance().GetSprite()->SetTransform(&_After.PastWorld);
					GraphicDevice::instance().GetSprite()->Draw(TexInfo->pTexture,
						&_srcRT, &__TextureCenter, nullptr,
						_After._Color);

					_After.T += _After.DeltaCoefft * dt;
					D3DXColorLerp(&_After._Color, &_After._Color, &_After._GoalColor, _After.T);

					if (_After._Color.a < 0)
					{
						_AfterIter = _RenderComp._AfterImgVec.erase(_AfterIter);
					}
					else ++_AfterIter;
				}
				_RenderComp.AfterImgPush(MWorld);
			}

			if (global::ECurGameState::PlaySlow == global::_CurGameState && _RenderComp.bSlowRender)
			{
				RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _RenderComp._Info.SrcScale.x,
						  spTexInfo->ImageInfo.Height * _RenderComp._Info.SrcScale.y };
				vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,
				spTexInfo->ImageInfo.Height / 2.f,0.f };
				GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
				GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture,
					&srcRect, &TextureCenter, nullptr,
					_RenderComp.SlowColor);

				// 그려지는 상황이므로 푸시
				if (global::IsPlay() && RecordManager::instance().bUpdate)
				{
					Record::Info _RecordInfo;
					int32_t CurTiming = RecordManager::instance().Timing;
					_RecordInfo.Alpha = 255;
					_RecordInfo.DrawID = _RenderComp._Info.GetCurFrame();
					_RecordInfo.MWorld = MWorld;
					_RecordInfo.ObjKey = _RenderComp._Info.ObjectKey;
					_RecordInfo.StateKey = _RenderComp._Info.StateKey;
					_RecordInfo.OwnerY = spOwner->_TransformComp->Position.y;
					_RecordInfo.Timing = CurTiming;
					_RecordInfo._Color = _RenderComp._Info._Color;

					_RenderComp._Record._Infos.insert({ CurTiming  , _RecordInfo });
				}
			}
			else
			{
				RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _RenderComp._Info.SrcScale.x,
						  spTexInfo->ImageInfo.Height * _RenderComp._Info.SrcScale.y };
				vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,
				spTexInfo->ImageInfo.Height / 2.f,0.f };
				GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
				GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture,
					&srcRect, &TextureCenter, nullptr,
					_RenderComp._Info._Color);

				// 그려지는 상황이므로 푸시
				if (global::IsPlay() && RecordManager::instance().bUpdate)
				{
					Record::Info _RecordInfo;
					int32_t CurTiming = RecordManager::instance().Timing;
					_RecordInfo.Alpha = 255;
					_RecordInfo.DrawID = _RenderComp._Info.GetCurFrame();
					_RecordInfo.MWorld = MWorld;
					_RecordInfo.ObjKey = _RenderComp._Info.ObjectKey;
					_RecordInfo.StateKey = _RenderComp._Info.StateKey;
					_RecordInfo.OwnerY = spOwner->_TransformComp->Position.y;
					_RecordInfo.Timing = CurTiming;
					_RecordInfo._Color = _RenderComp._Info._Color;

					_RenderComp._Record._Infos.insert({ CurTiming  , _RecordInfo });
				}
			}
		}

		auto NotifyEvent = _RenderComp._Info._Nofify.find(_RenderComp._Info.GetCurFrame());

		if (NotifyEvent != std::end(_RenderComp._Info._Nofify))
		{
			if (NotifyEvent->second)
			{
				// 해당 프레임에 도달하였으니 함수 호출
				NotifyEvent->second();
			}
		}

		if (_RenderComp._RenderAfterEvent)
		{
			_RenderComp._RenderAfterEvent();
		}
	};

	return GunRender;
};

OBJECT_ID::EID Gangster::GetID()
{
    return OBJECT_ID::EID::GANGSTER;
}

OBJECT_TAG::ETAG Gangster::GetTag()
{
    return Super::GetTag();
}

std::wstring_view Gangster::GetName() const&
{
    return L"Gangster"sv;
}

void Gangster::Initialize() & noexcept
{
    Super::Initialize();

	_SpGun = ComponentManager::instance().Insert<RenderComponent>(_This);
	_SpGun->bRender = false;
	_SpGun->AfterImgOff();
	_SpGun->PositionCorrection = vec3{ 0.f,-0,0.f };
	_SpGun->Anim(false, false, L"spr_gangstergun", 1, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 }, L"Gangster", LAYER::ELAYER::EOBJECT);
	_SpGun->_Control.bRender = true;
	_SpGun->_Control._Render = RunRenderMake(GunRotZ);

	_TransformComp->Scale *= 2.5f;
	_PhysicComp->Mass = 100.f;

	_RenderComp->bRender = true;

	_RenderComp->AfterImgOff();
	_RenderComp->PositionCorrection = vec3{ 0.f,-10.f,0.f };

	_CollisionComp->_CollisionInfo._ShapeType = CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 35;
	_CollisionComp->_CollisionInfo.Width = 18;

	_PhysicComp->bGravity = true;

	Speed = 500.f;
	MoveGoalTime = 2.f;
	DetectionRange = 400.f;
	AttackRange = 500.f;

	_CurrentState = Gangster::State::Idle;

	PursuitRange = 700.f;
	NarrowRange = 180.f;
	//TODO::
	/*_SpAttack = ObjectManager::instance().InsertObject < Grunt_Slash>();
	_SpAttack->SetOwner(_This);*/
	DelayAfterAttack = 0.4f;

}

void Gangster::Update()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;

	Super::Update();

	FSM();
}

void Gangster::LateUpdate()
{
	if (!global::IsPlay())return;
	if (!ObjectManager::instance().bEnemyUpdate)return;


	Super::LateUpdate();

	const float Dt = Time::instance().Delta();
}

void Gangster::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CurrentState == Gangster::State::Walk && _CollisionInfo._ID == OBJECT_ID::ETILE)
	{
		if (
			(math::almost_equal(vec3{ 1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &vec3{ 1.f,0.f,0.f }) > cosf(math::PI / 5.f))) ||
			(math::almost_equal(vec3{ -1.f,0.f,0.f }, _CollisionInfo.Normal)
				&& (D3DXVec3Dot(&_CollisionInfo.PosDir, &vec3{ -1.f,0.f,0.f }) > cosf(math::PI / 5.f)))
			)
		{
			Turn();
			_PhysicComp->Dir.x *= -1.f;
		}
	}
}

void Gangster::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target, _CollisionInfo);


}

void Gangster::Move(vec3 Dir, const float AddSpeed)
{
	Super::Move(Dir, AddSpeed);

}

void Gangster::Die()&
{
	HurtFly();

	bBlooding = true;

}

void Gangster::EnterStair()
{
	_CurrentState = Gangster::State::EnterStair;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]()
	{
		bEnterStairMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_gangster_enterstair", 6, 0.66, std::move(_Notify));
}

void Gangster::LeaveStair()
{
	_CurrentState = Gangster::State::LeaveStair;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]()
	{
		bLeaveStairMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_gangster_leavestair", 6, 0.66f, std::move(_Notify));
}

void Gangster::EnterStairState()
{
	if (bEnterStairMotionEnd)
	{
		bEnterStairMotionEnd = false;
		_PhysicComp->Position = LeaveDoorLocation;
		LeaveStair();
	}
}

void Gangster::LeaveStairState()
{
	if (bLeaveStairMotionEnd)
	{
		bLeaveStairMotionEnd = false;
		Run();
	}
}

void Gangster::Attack()
{
	constexpr float AttackRich = 50.f;
	constexpr float BulletSpeed = 700.f;

	_CurrentState = Gangster::State::Attack;
	RenderComponent::NotifyType _Notify;
	vec3 ToTarget = _Target->_TransformComp->Position;
	ToTarget -= _PhysicComp->Position;
	D3DXVec3Normalize(&ToTarget, &ToTarget);

	GunRotZ = atan2f(ToTarget.y, ToTarget.x);

	_SpGun->bRender = true;
	/* X축 방향만 필요하다면 사용 아니라면 사용 X ToTarget = ConvertXAxisDir(ToTarget);
	TODO :: */
	_Notify[7] = [ToTarget, this]()
	{
		bAttackMotionEnd = true;
		//TODO :: 
		//_SpAttack->AttackStart(ToTarget * AttackRich, ToTarget);
		vec3 FireLocation = _PhysicComp->Position + (ToTarget * AttackRich);

		int32_t FireEffectImgID = math::Rand<int32_t>({ 0,2 });
		std::wstring FireEftKey = L"spr_fire_" + std::to_wstring(FireEffectImgID);

		EffectManager::instance().EffectPush(L"Effect", L"spr_bullet",
			1, (std::numeric_limits<float>::max)(), 10.f, OBJECT_ID::EID::BULLET,
			true, FireLocation, ToTarget * BulletSpeed, { 1,1,1 }, false,
			true, false, true, 34, 2, 255, false, 0.f, atan2f(ToTarget.y, ToTarget.x),
			0);

		EffectManager::instance().EffectPush(L"Effect", FireEftKey, 6, 0.1f, 0.1f * 6 + 0.01f,
			OBJECT_ID::EID::ENONE, true, FireLocation, { 0,0,0 }, { 2,2,2, });
	};
	_RenderComp->Anim(false, false, L"spr_gangsteraim", 7, 0.1f, std::move(_Notify));
	AtTheAttackDir = ToTarget;
}

void Gangster::AttackState()
{
	if (bAttackMotionEnd)
	{
		bAttackMotionEnd = false;

		vec3 ToTarget = _Target->_PhysicComp->Position - _PhysicComp->Position;

		if (D3DXVec3Dot(&AtTheAttackDir, &ToTarget) < 0.f)
		{
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			_SpGun->bRender = false;
			Turn();
			return;
		};

		Time::instance().TimerRegist(DelayAfterAttack, DelayAfterAttack, DelayAfterAttack,
			[this]() {
			if (!global::IsPlay())return true;
			if (_EnemyState != NormalEnemy::State::Die)
			{
				_SpGun->bRender = false;
				Run();
			}
			return true;
		});
	}
}

void Gangster::Fall()
{
}

void Gangster::FallState()
{
}

void Gangster::HurtFly()
{
	_CurrentState = Gangster::State::HurtFly;
	_RenderComp->Anim(false, false, L"spr_gangsterhurtfly", 2, 0.5f);
	_PhysicComp->Flying();
}

void Gangster::HurtFlyState()
{
	if (_PhysicComp->bLand)
	{
		bBlooding = false;
		HurtGround();
	}
}

void Gangster::HurtGround()
{
	_CurrentState = Gangster::State::HurtGround;
	RenderComponent::NotifyType _Notify;
	bBloodingOverHead = true;

	_Notify[14] = [this]()
	{
		_RenderComp->bSlowRender = true;
		bHurtGroundMotionEnd = true;
	};

	_Notify[8] = [this]() {bBloodingOverHead = false; };

	_RenderComp->Anim(false, false, L"spr_gangsterhurtground",
		14, 1.8f, std::move(_Notify));
}

void Gangster::HurtGroundState()
{
	if (bHurtGroundMotionEnd)
	{
		bHurtGroundMotionEnd = false;
	}
}

void Gangster::Idle()
{
	_CurrentState = Gangster::State::Idle;
	_RenderComp->Anim(false, true, L"spr_gangsteridle", 8, 0.5f);
}

void Gangster::IdleState()
{
	vec3 BeforeDir = _PhysicComp->Dir;
	IsDetected = IsRangeInnerTarget();

	if (IsDetected)
	{
		if (D3DXVec3Dot(&BeforeDir, &ToTarget) < 0.f)
		{
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			Turn();
			return;
		}
		else
			Run();
	}
}

void Gangster::Run()
{
	if (!global::IsPlay())return;

	_CurrentState = Gangster::State::Run;
	_RenderComp->Anim(false, true, L"spr_gangsterrun", 10, 0.6f);
	bLaziness = false;
}

void Gangster::RunState()
{
	vec3 ToTarget = _Target->_TransformComp->Position - _PhysicComp->Position;
	float ToTargetDistance = D3DXVec3Length(&ToTarget);
	if (ToTargetDistance < AttackRange)
	{
		_SpGun->bRender = true;
		Attack();
		return;
	};

	vec3 GoalTargetDiff = GoalPos - _Target->_TransformComp->Position;
	float Distance = D3DXVec3Length(&GoalTargetDiff);

	if (GoalPos == vec3{ 0.f,0.f,0.f })
	{
		Paths = AStarManager::instance().PathFind(_PhysicComp->Position, _Target->_TransformComp->Position);
		if (!Paths.empty())
		{
			FollowRouteProcedure();
		}
		return;
	}
	else if (Distance > PathFindCheckMinDistanceMin &&
		abs(GoalPos.x - _Target->_TransformComp->Position.x) > 100.f)
	{
		Paths = AStarManager::instance().PathFind(_PhysicComp->Position, _Target->_TransformComp->Position);
		if (!Paths.empty())
		{
			FollowRouteProcedure();
		}
		return;
	}

	if (Paths.empty())return;

	vec3 ToMoveMark = CurMoveMark - _PhysicComp->Position;
	if (std::abs(ToMoveMark.x) < NextPathCheckMinDistance)
	{
		FollowRouteProcedure();
	}

	Move(_PhysicComp->Dir, Speed);
}

void Gangster::Turn()
{
	_CurrentState = Gangster::State::Turn;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]() {
		bTurnMotionEnd = true;
	};
	_RenderComp->Anim(true, false, L"spr_gangsterturn", 6, 0.53f, std::move(_Notify));
}

void Gangster::TurnState()
{
	if (bTurnMotionEnd)
	{
		bTurnMotionEnd = false;

		switch (_EnemyState)
		{
		case NormalEnemy::State::Idle:
			Idle();
			break;
		case NormalEnemy::State::Detecting:
			Run();
			break;
		case NormalEnemy::State::Walk:
			Walk();
			break;
		default:
			Idle();
			break;
		}
	}
}

void Gangster::Walk()
{
	_EnemyState = NormalEnemy::State::Walk;
	_CurrentState = Gangster::State::Walk;
	_RenderComp->Anim(false, true, L"spr_gangsterwalk",
		8, 0.84f);
}

void Gangster::WalkState()
{
	vec3 BeforeDir = _PhysicComp->Dir;
	IsDetected = IsRangeInnerTarget();

	if (IsDetected)
	{
		if (D3DXVec3Dot(&BeforeDir, &ToTarget) < 0.f)
		{
			_PhysicComp->Dir = ConvertXAxisDir(ToTarget);
			Turn();
			return;
		}
		else
		{
			Run();
		}
	}

	Move(_PhysicComp->Dir, Speed * 0.25f);
}

void Gangster::AnyState()
{
}

void Gangster::FSM()
{
	switch (_CurrentState)
	{
	case Gangster::State::Attack:
		AttackState();
		break;
	case Gangster::State::Fall:
		FallState();
		break;
	case Gangster::State::HurtFly:
		HurtFlyState();
		break;
	case Gangster::State::HurtGround:
		HurtGroundState();
		break;
	case Gangster::State::Idle:
		IdleState();
		break;
	case Gangster::State::Run:
		RunState();
		break;
	case Gangster::State::Turn:
		TurnState();
		break;
	case Gangster::State::Walk:
		WalkState();
		break;
	case Gangster::State::LeaveStair:
		LeaveStairState();
		break;
	case Gangster::State::EnterStair:
		EnterStairState();
		break;
	default:
		break;
	}

	AnyState();
}

void Gangster::FollowRouteProcedure()
{
	CurMoveMark = Paths.back();
	GoalPos = Paths.front();
	Paths.pop_back();

	vec3 ToPath = CurMoveMark - _PhysicComp->Position;
	_Y = ToPath.y;
	if (ToPath.y > 40.f)
	{
		_CollisionComp->bDownJump = true;
	}
	else
	{
		_CollisionComp->bDownJump = false;
	}

	if (D3DXVec3Dot(&ConvertXAxisDir(_PhysicComp->Dir), &ConvertXAxisDir(ToPath)) < 0.f)
	{
		_PhysicComp->Dir = ConvertXAxisDir(ToPath);
		Turn();
		return;
	}

	if (std::abs(ToPath.y) > DoorPathCheckMinDistance)
	{
		LeaveDoorLocation = CurMoveMark;
		LeaveDoorLocation.y -= 30.f;
		EnterStair();
		return;
	}
	else
	{
		_PhysicComp->Dir = ConvertXAxisDir(ToPath);
	}
}

void Gangster::SetUpInitState(float DirX, int32_t StateID)
{	// TODO :: 여기서 초기 상태 지정 ..
	_RenderComp->Anim(false, true, L"spr_gangsteridle",
		8, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, 
	L"Gangster", LAYER::ELAYER::EOBJECT);

	switch (StateID)
	{
	case 0:
		Idle();
		/*_RenderComp->Anim(false, true, L"spr_grunt_lean",
			1, 0.5f, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, vec2{ 1.f,1.f }, L"Grunt", LAYER::ELAYER::EOBJECT);*/
		// 게으른 순찰상태.
		bLaziness = true;
		break;
	case 1:
		Idle();
		break;
	case 2:
		Walk();
		break;
	default:
		Idle();
		break;
	};

	// TOOD :: 여기서 초기 방향 지정 ..... 
	AtTheAttackDir = _PhysicComp->Dir = vec3{ DirX,0.f,0.f };
};
