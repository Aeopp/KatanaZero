#include "stdafx.h"
#include "Player.h"
#include "ComponentManager.h"
#include "RenderComponent.h"
#include "InputManager.h"
#include "Time.h"
#include "ObjectManager.h"
#include "Camera.h"
#include "CollisionComponent.h"
#include "Battery.h"
#include "UIItemIcon.h"
#include "PhysicTransformComponent.h"
#include "AttackSlash.h"
#include "EffectManager.h"
#include "math.h"
#include "global.h"
#include "AStarManager.h"
#include "EffectManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "Time.h"
#include "Item.h"
#include "Texture_Manager.h"
#include "GraphicDevice.h"
#include "Door.h"
#include "Boss.h"
#include "PanicSwitch.h"
#include "sound_mgr.h"



using namespace std;

OBJECT_ID::EID Player::GetID()
{
	return OBJECT_ID::EPLAYER;
};

OBJECT_TAG::ETAG Player::GetTag()
{
	return Super::GetTag();
};

std::wstring_view Player::GetName() const&
{
	return L"Player"sv;
};

void Player::Initialize() & noexcept
{
	Super::Initialize();

	_TransformComp->Scale *= 2.2f;
	_PhysicComp->Mass = 100.f;
	_RenderComp->Anim(false, true,
		L"spr_dragon_idle", 12, 1.f,
		{}, D3DCOLOR_ARGB(255, 255, 255, 255),
		0.f, vec2{ 1.f,1.f }, L"Dragon",
		LAYER::ELAYER::EOBJECT);
	_RenderComp->AfterImgOff();
	_RenderComp->PositionCorrection = vec3{ 0.f,-8.f,0.f };
	_RenderComp->SlowDeltaCoefft = 0.05f;
	_RenderComp->NormalAfterImgPushDelta *= 1.f;
	
	_CollisionComp->_CollisionInfo._ShapeType =
		CollisionComponent::CollisionInfo::EShapeType::Rect;
	_CollisionComp->_CollisionInfo.Height = 35;
	_CollisionComp->_CollisionInfo.Width = 18;
	_CollisionComp->_Tag = CollisionComponent::ETag::EPlayer;

	KeyBinding();

	// TODO :: 아이템 획득시 배터리에게 통보해주기.
	_SpBattery = ObjectManager::instance().InsertObject<Battery>();
	_SpBattery->SetOwner(_This);

	_SpUIItemIcon = ObjectManager::instance().InsertObject<UIItemIcon>();
	_SpUIItemIcon->SetOwner(_This);
	
	_SpAttackSlash = ObjectManager::instance().InsertObject<typename Attack_Slash>();
	_SpAttackSlash->SetOwner(_This);

	_PhysicComp->bGravity = true;

	_PhysicComp->bFollowOwner = false;

	Speed = PlayerSpeed;
	MoveGoalTime = 2.f;
	TimeRegist();

	/*PlayStartColor = D3DCOLOR_ARGB(125, 255, 0, 0);
	PlayGoalColor = D3DCOLOR_ARGB(0, 0, 255, 255);
	SlowStartColor = D3DCOLOR_ARGB(125, 0, 0, 255);
	SlowGoalColor = D3DCOLOR_ARGB(0, 255, 0, 255);*/

	Time::instance().TimerRegist(0.0f, 0.01f, 1.f, [This = _This, this]() {
		if (This.expired())return true;
		vec3 Pos = _TransformComp->Position;
		Pos.x -= global::ClientSize.first / 2.f;
		Pos.y -= global::ClientSize.second / 2.f;
		global::CameraPos = Pos;

		return true;
	});
	/*_RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 255, 0, 255);*/

	_RenderComp->_RenderAfterEvent = [this]()
	{	
		const float JoomScale = global::JoomScale;
		vec3 CameraPos = global::CameraPos;

		matrix MWorld = this->_TransformComp->CalcWorldMatrix(true);

		MWorld = MWorld * math::GetCameraJoomMatrix(JoomScale, vec3{ global::ClientSize.first,global::ClientSize.second,0.f });
		MWorld._41 += _RenderComp->PositionCorrection.x;
		MWorld._42 += _RenderComp->PositionCorrection.y;
		MWorld._43 += _RenderComp->PositionCorrection.z;

		// 대쉬 쿨타임 렌더링 시작 ... 
		if (CurDashCoolTime > 0.f)
		{
			vec2 CoolTimeRange = { 30,0 };
			vec2 CoolTimeLocationRePos = { 0,-70};
			CoolTimeLocationRePos *= JoomScale;
			CoolTimeRange *= JoomScale;

			vec2 PlayerLocation = { MWorld._41,MWorld._42 };
			vec2 CoolLineStartLocation = PlayerLocation + CoolTimeLocationRePos;
			
			std::array<vec2, 2ul> _CoolTimeLeftRight;

			_CoolTimeLeftRight[0] = CoolLineStartLocation + (-CoolTimeRange * (CurDashCoolTime / DashCoolTime));
			_CoolTimeLeftRight[1]= CoolLineStartLocation + (+CoolTimeRange *(CurDashCoolTime / DashCoolTime));

			GraphicDevice::instance().GetSprite()->End();
			GraphicDevice::instance().GetLine()->SetWidth(15.f);
			GraphicDevice::instance().GetLine()->Draw(_CoolTimeLeftRight.data(), _CoolTimeLeftRight.size(), D3DCOLOR_ARGB(255, 255, 0, 0));
			GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
		}
		// 대쉬 레인지 렌더링 시작 ....
		if (!bDashRangeRender) return;

		auto spTexInfo = TextureManager::instance().Get_TexInfo
		(L"Dragon", L"spr_dragon_dash_range", 0);
		
		vec2 LineStartLocation = { MWorld._41  ,   	MWorld._42 };
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
			RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _RenderComp->_Info.SrcScale.x,
					  spTexInfo->ImageInfo.Height * _RenderComp->_Info.SrcScale.y };
			vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,
			spTexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture,
				&srcRect, &TextureCenter, nullptr,
				D3DCOLOR_ARGB(255,255,255,255));
		}

		vec3 MouseLocation  = global::MousePosWorld;
		vec3 MyLocation = _PhysicComp->Position;
		vec3 ToMouse = MouseLocation - MyLocation;
		CurDashRange = D3DXVec3Length(&ToMouse);
		CurDashRange  = min(CurDashRange, 500.f);
		vec3 ToMouseDir;
		D3DXVec3Normalize(&ToMouseDir, &ToMouse);
		//float DashRange = 500.f;
		ToMouseDir *= CurDashRange;
		if (ToMouseDir.x < 0.f)
		{
			MWorld._11 *= -1.f;
		}

		MWorld._41 += ToMouseDir.x;
		MWorld._42 += ToMouseDir.y;

		{
			vec2 ToMouseDir2D = { ToMouseDir.x,ToMouseDir.y };
			D3DXVec2Normalize(&ToMouseDir2D, &ToMouseDir2D);
			ToMouseDir2D *= CurDashRange;
			std::array<vec2, 2ul> Line2Ds{ LineStartLocation ,   ToMouseDir2D + vec2{ LineStartLocation.x,LineStartLocation.y } };
			GraphicDevice::instance().GetSprite()->End();
			GraphicDevice::instance().GetLine()->SetWidth(8.f);
			GraphicDevice::instance().GetLine()->Draw(Line2Ds.data(), Line2Ds.size(), D3DCOLOR_ARGB(255, 0, 199, 235));
			GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
		}

		{
			auto spTexInfo = TextureManager::instance().Get_TexInfo
			(L"Dragon", L"spr_dragon_attack", 6);

			RECT srcRect = { 0,0,spTexInfo->ImageInfo.Width * _RenderComp->_Info.SrcScale.x,
					  spTexInfo->ImageInfo.Height * _RenderComp->_Info.SrcScale.y };
			vec3 TextureCenter = { spTexInfo->ImageInfo.Width / 2.f,
			spTexInfo->ImageInfo.Height / 2.f,0.f };
			GraphicDevice::instance().GetSprite()->SetTransform(&MWorld);
			GraphicDevice::instance().GetSprite()->Draw(spTexInfo->pTexture,
				&srcRect, &TextureCenter, nullptr,
				D3DCOLOR_ARGB(255, 0, 199, 235));
		}
	};
};

void Player::LateInitialize() & noexcept
{
	Super::LateInitialize();
}

void Player::Release() & noexcept
{
	Super::Release();
}

void Player::Update()
{
	if (!bInit)return;
	Super::Update();

	FSM();
}

void Player::LateUpdate()
{
	if (!bInit)return;

	Super::LateUpdate();

	const float Dt = Time::instance().Delta();

	bFrameCurrentCharacterInput = false;
	bMoveKeyCheck = false;
	bJumpKeyCheck = false; 
	bAttackKeyCheck = false;
	bDownKeyCheck = false;
	bSneakKeyCheck = false;
	bCurWallRideCollision = false;
	

	RollEffectDelta -= Dt;
	WallRideEffectDelta -= Dt;
	CurAttackCoolTime -= Dt;
	InvincibleTime -= Dt;
	CurDashCoolTime -= Dt; 
	CurRunSoundRepeatTime -= Dt;
	CurWallSlideTime -= Dt;
	CurFootStepSoundTime -= Dt;

	if (!_SpBattery->IsUse())
	{
		if (global::IsSlow())
		{
			Time::instance().Return();
		}
	}
};

void Player::MapHit(typename math::Collision::HitInfo _CollisionInfo)
{
	Super::MapHit(_CollisionInfo);

	if (_CollisionInfo._ID == OBJECT_ID::EWALLRIDELINE && _PhysicComp->bFly)
	{
		const vec3 UpVec = { 0.f,-1.f,0.f };
		FlipDir = *_CollisionInfo._Variable._Cast<vec3>();
		D3DXVec3Lerp(&FlipDir, &FlipDir, &UpVec, 0.5f);
		bCurWallRideCollision = true;
		CurWallRideLinePos = _CollisionInfo.Position;
		FSM();
	}

	/*if (_CollisionInfo._ID == OBJECT_ID::ETILE)
	{
		if (_CollisionInfo.Normal)
		{
			if (math::almost_equal(vec3{ 1.f,0.f,0.f }, _CollisionInfo.Normal))
			{
				int i = 0;
			}
		}
	}*/
}
void Player::Hit(std::weak_ptr<class object> _Target, math::Collision::HitInfo _CollisionInfo)
{
	Super::Hit(_Target, _CollisionInfo);
	if (IsInvisible())return;
	if (bHurt)return;

	if (_CollisionInfo._ID == OBJECT_ID::LASER_TRAP  )
	{
		if (global::IsSlow())
		{
			Time::instance().Return();
		}
		bFatal = true;
		bHurt = true;


		ObjectManager::instance()._Camera.lock()->CameraShake(
			500.f, math::Rand<int32_t>({ 0,1 }) ? vec3{ 1,0,0 } : vec3{ -1,0,0 }, 0.7f);
		
		Time::instance().TimeScale = 0.2f;
		Time::instance().TimerRegist(0.02f, 0.02f, 0.02f, [this]() {

			if (global::ECurGameState::PlaySlow != global::_CurGameState)
			{
				Time::instance().TimeScale = 1.f;
			}
			return true; });

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

		SOUNDPLAY("sound_laser_explosion", 0.75f, false);
		

		_CurrentState = Player::State::Hurt_Ground;
		bHurtGroundMotionEnd = true;
		ReWindStart();
	}

	if (_CollisionInfo._ID == OBJECT_ID::BOSS_RECOVER_BOMB)
	{
		if (global::IsSlow())
		{
			Time::instance().Return();
		}

		vec3 UpDir = { 0,-1,0 };
		vec3 Dir =_TransformComp->Position - ObjectManager::instance()._Boss.lock()->_TransformComp->Position;
		D3DXVec3Normalize(&Dir, &Dir);

		if (Dir.y > 0.f)Dir.y *= -1.f;

		_CollisionInfo.PosDir = Dir;

		D3DXVec3Lerp(&_CollisionInfo.PosDir, &_CollisionInfo.PosDir,&UpDir, 0.25f);
		
		_CollisionInfo.PushForce = 400.f;

		_PhysicComp->Move((_CollisionInfo.PosDir) * (_CollisionInfo.PushForce * 4.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PosDir);

		float ImpactRotZ = atan2f(-_CollisionInfo.PosDir.y, -_CollisionInfo.PosDir.x);

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PosDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PosDir * 4000);

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce * 2.f, _CollisionInfo.PosDir, 0.3f);

		Time::instance().TimeScale = 0.2f;
		Time::instance().TimerRegist(0.02f, 0.02f, 0.02f, [this]() {

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

		HurtFlyBegin();
	}

	if (_CollisionInfo._ID == OBJECT_ID::EID::EXPLOSION)
	{
		if (global::IsSlow())
		{
			Time::instance().Return();
		}
		_CollisionInfo.PushDir = { 0,-1,0 };
		_CollisionInfo.PushDir += _CollisionInfo.PosDir;
		D3DXVec3Normalize(&_CollisionInfo.PushDir, &_CollisionInfo.PushDir);

		_CollisionInfo.PushForce = 500.f;

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 2.f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);
		BloodInit(_CollisionInfo.PushDir);

		if (!bCheat)
			bFatal = true;
		sound_mgr::instance().Play("dragon_death", false, 0.6f);
		SOUNDPLAY("playerdie", 1.f, false);

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

		HurtFlyBegin();
		return;
	}

	if (_CollisionInfo._ID == OBJECT_ID::EID::BOSS_LASER)
	{
		if (global::IsSlow())
		{
			Time::instance().Return();
		}

		_CollisionInfo.PushDir = _CollisionInfo.PosDir;
		_CollisionInfo.PushForce = 700;

		_CollisionInfo.PushDir.y -= 0.6f;

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 1.6f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		BloodInit(_CollisionInfo.PushDir);

		if (!bCheat)
			bFatal = true;
		sound_mgr::instance().Play("dragon_death", false, 0.6f);
		SOUNDPLAY("playerdie", 1.f, false);

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);
	

		EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);

		{
			vec3 Location = _TransformComp->Position;
			float ExplosiveRepeatTime = 0.1f;
			RAND_SOUNDPLAY("explosion", { 1,3 }, 1.f);

			for (int i = 0; i < 15; ++i)
			{
				vec3 InitLoc = Location + math::RandVec({ -1,1 }) * math::Rand<float>({40,140});
				EffectManager::instance().EffectPush(L"Effect", L"spr_explosion_1",
					11, 0.1f, 11 * 0.1f + 0.01f, OBJECT_ID::EID::EXPLOSION, true,
					InitLoc, { 0,0,0 },
					{ 3,3,0 }, false, true, false, true, 33, 33,
					255, false, 0.f, 0.f, 0.f, 0);
				

			}
		}

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce * 9, _CollisionInfo.PushDir, 0.3f);

		Time::instance().TimeScale = 0.2f;
		Time::instance().TimerRegist(0.08f, 0.08f, 0.08f, [this]() {

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

		HurtFlyBegin();
	}
	if (_CollisionInfo._ID == OBJECT_ID::EID::BOSS)
	{
		auto spObj =_CollisionInfo._Target.lock();
		auto spBoss = std::dynamic_pointer_cast<Boss>(spObj);

		if (spBoss->IsAttacking())
		{
			if (global::IsSlow())
			{
				Time::instance().Return();
			}

			_CollisionInfo.PushDir = _CollisionInfo.PosDir;
			_CollisionInfo.PushForce = 400;

			_CollisionInfo.PushDir.y -= 0.f;

			vec3 UpDir = { 0,-1,0 };

			/*_CollisionInfo.PushDir =*/ D3DXVec3Lerp(&_CollisionInfo.PushDir, &_CollisionInfo.PushDir,
				&UpDir, 0.5f);

			 //_CollisionInfo.PushDir.y -= 0.3f;

			_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 2.5f),
				_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
				0.3f,
				_CollisionInfo.PushDir);

			BloodInit(_CollisionInfo.PushDir);
			sound_mgr::instance().Play("dragon_death", false, 0.6f);
			SOUNDPLAY("playerdie", 1.f, false);

			if (!bCheat)
				bFatal = true;


			EffectManager::instance().EffectPush(L"Effect",
				L"spr_slashfx", 5, 0.08f,
				0.08f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false, _PhysicComp->Position,
				{ 0,0,0 }, { 2.9,2.9,0 });

			float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
			EffectManager::instance().EffectPush(L"Effect",
				L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
				_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
				{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
				255, false, 0, ImpactRotZ, 0, 0);

			vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

			EffectManager::instance().EffectPush(L"Effect",
				L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
				_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
				, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
				0, 0);


			ObjectManager::instance()._Camera.lock()->CameraShake(
				_CollisionInfo.PushForce * 3, _CollisionInfo.PushDir, 0.3f);

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

			HurtFlyBegin();
			// 여기서 피격 처리.........
		}
	}

	if (!bInAreaSmoke && _CollisionInfo._ID == OBJECT_ID::EID::SMOKE_CLOUD)
	{
		bInAreaSmoke = true;

		Time::instance().TimerRegist(0.3f, 0.3f, 0.3f, [this,This = _This]() { 
			if (This.expired())return true;
			bInAreaSmoke = false;
			return true; });
	}
	if (_CollisionInfo._ID == OBJECT_ID::EID::DOOR)
	{
		_CurDoor = _CollisionInfo._Target;
	}

	if (_CollisionInfo._ID == OBJECT_ID::EID::BULLET)
	{
		auto _RefEftInfo = _CollisionInfo._Variable._Cast<std::reference_wrapper<EffectInfo>>();
		_RefEftInfo->get().T = 0.f;
		_RefEftInfo->get().MaxT = 5.f;
		_RefEftInfo->get().bPhysic = false;
		_RefEftInfo->get().Scale.x *= 4.f;

		RAND_SOUNDPLAY("sound_bullethit", { 1,3 }, 1.f);

		if (_CurrentState == Player::State::Attack && AtAttackDir.has_value())
		{
			vec3 BulletDir = _CollisionInfo.PosDir;
			float dot = D3DXVec3Dot(&BulletDir, &*AtAttackDir);
			if (dot < 0.f)
				return;
		}
		_CollisionInfo.PushDir = _CollisionInfo.PosDir;
		_CollisionInfo.PushForce = 700;

		_CollisionInfo.PushDir.y -= 0.4f;

		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 2.5f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		if (global::IsSlow())
		{
			Time::instance().Return();
		}

		BloodInit(_CollisionInfo.PushDir);
		sound_mgr::instance().Play("dragon_death", false, 0.6f);
		SOUNDPLAY("playerdie", 1.f, false);

		if(!bCheat)
			bFatal = true;

		/*EffectManager::instance().EffectPush(L"Effect",
			L"spr_slashfx", 5, 0.02f,
			0.02f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false, _PhysicComp->Position,
			{ 0,0,0 }, { 2.9,2.9,0 });*/

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

		EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce * 5, _CollisionInfo.PushDir, 0.3f);

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

		HurtFlyBegin();
	}
	else if (_CollisionInfo._TAG == OBJECT_TAG::ENEMY_ATTACK  )
	{
		vec3 UpDir = { 0,-1,0 };

		/*_CollisionInfo.PushDir =*/ D3DXVec3Lerp(&_CollisionInfo.PushDir, &_CollisionInfo.PushDir,
			&UpDir, 0.3f);

		// _CollisionInfo.PushDir.y -= 0.4f;
		
		_PhysicComp->Move((_CollisionInfo.PushDir) * (_CollisionInfo.PushForce * 10.f),
			_CollisionInfo.IntersectAreaScale * _CollisionInfo.PushForce * 0.01f,
			0.3f,
			_CollisionInfo.PushDir);

		if (global::IsSlow())
		{
			Time::instance().Return();
		}

		BloodInit(_CollisionInfo.PushDir);
		sound_mgr::instance().Play("dragon_death", false, 0.6f);
		SOUNDPLAY("playerdie", 1.f, false);
		SOUNDPLAY("punch_hit", 0.85f, false);
		
		if(!bCheat)
			bFatal = true;

		EffectManager::instance().EffectPush(L"Effect",
			L"spr_slashfx", 5, 0.08f,
			0.08f * 5 + 0.01f, OBJECT_ID::EID::SLASH_FX, false, _PhysicComp->Position,
			{ 0,0,0 }, { 2.9,2.9,0 });

		float ImpactRotZ = atan2f(-_CollisionInfo.PushDir.y, -_CollisionInfo.PushDir.x);
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_hit_impact", 5, 0.08f, 0.08f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
			_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
			{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
			255, false, 0, ImpactRotZ, 0, 0);

		vec3 Pos = _PhysicComp->Position + (-_CollisionInfo.PushDir * 4000);

		EffectManager::instance().EffectPush(L"Effect",
			L"HitEffect", 1, 0.2f, 0.201f, OBJECT_ID::EID::HIT_EFFECT, false, Pos,
			_CollisionInfo.PushDir * 50000, { 50,3,0 }, false, false, false, false
			, 0, 0, 125, true, 0, atan2f(_CollisionInfo.PushDir.y, _CollisionInfo.PushDir.x),
			0, 0);

		ObjectManager::instance()._Camera.lock()->CameraShake(
			_CollisionInfo.PushForce * 5, _CollisionInfo.PushDir, 0.3f);

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

		HurtFlyBegin();
	};
};

void Player::Move(vec3 Dir,const float AddSpeed)
{
	switch (_CurrentState)
	{
	case Player::State::Sneak:
		Speed = PlayerSpeed * 0.5f;
		break; 
	case Player::State::Fall:
		Speed = PlayerSpeed * 0.40f;
		break;
	case Player::State::Jump:
		Speed = PlayerSpeed * 0.70f;
		break;
	case Player::State::Run:
		Speed = PlayerSpeed;
		break;
	case Player::State::Roll:
		Speed = PlayerSpeed * 1.25f;
		break;
	default:
		break;
	}

	Super::Move(Dir, AddSpeed);
}

bool Player::IsAlive() const&
{
	return _CurrentState !=Player::State::Hurt_Ground;
}

void Player::Idle()
{
	_RenderComp->PositionCorrection = vec3{ 0.f,-8.f,0.f };
	_CurrentState = Player::State::Idle;
	_RenderComp->Anim(false, true, L"spr_dragon_idle", 12, 1.f);
}

void Player::IdleState()
{
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
		return; 
	}
	if (bSneakKeyCheck && bMoveKeyCheck)
	{
		Sneak();
	}
	else if (bMoveKeyCheck)
	{
		IdleToRun();
	}
	if (bJumpKeyCheck && !_PhysicComp->bFly)
	{
		Jump();
		return;
	}
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		PreCrouch();
	}
}

void Player::IdleToRun()
{
	_CurrentState = Player::State::Idle_to_Run;

	RenderComponent::NotifyType _Notify;
	_Notify[3] = [this]()
	{bIdleToRunMotionEnd = true; };
	_RenderComp->Anim(false, false, L"spr_dragon_idle_to_run", 3, 0.12f , std::move(_Notify));

	vec3 InitPos = _PhysicComp->Position + vec3{ 0,15.f,0.f };
	vec3 InitDir = -_PhysicComp->Dir * 200.f;
	float XDir = -_PhysicComp->Dir.x;

	Time::instance().TimerRegist(0.07, 0.07, 0.07,
		[this, InitPos]()
	{
		EffectManager::instance().EffectPush(
			L"Effect", L"spr_stompercloud2",
			10, 0.05f, 10 * 0.05f + 0.01f, OBJECT_ID::EID::STOMPER_CLOUD, true,
			InitPos,
			vec3{ 0.f,-1.f,0.f }, { -3 * _PhysicComp->Dir.x
			,3,0.f },
			false, false, false, false, 0.f, 0.f,
			255, false, 0.f, 0.f, 0.f, 0);
		return true;
	});
	
	InitPos.y += 15.f;

	Time::instance().TimerRegist(0.07f, 0.02f, 0.07f + 0.02f * 7,
		[this, InitPos, InitDir, XDir]()mutable ->bool
	{
		vec3 Dir = math::RotationVec(InitDir, math::Rand<float>({ 0,100 }) * -XDir);
		vec3 Pos = InitPos +  ((vec3{ math::Rand<float>({0.f,1.f}),0,0 }*100) * XDir);

		EffectManager::instance().EffectPush(L"Effect", L"spr_dustcloud",
			7, 0.05f, 7 * 0.05f + 0.01f, OBJECT_ID::EID::DustCloud, true, Pos, Dir, { 3,3,0 },
			false, false, false, false, 0.f, 0.f, 255, false, 0.f, 0.f, 0.f, 0);
		return false;
	});

	SOUNDPLAY("player_prerun", 0.35f, false);
}

void Player::IdleToRunState()
{
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
		return;
	}
	if (!bMoveKeyCheck)
	{
		Idle(); 
	}
	if (bIdleToRunMotionEnd)
	{
		Run();
		bIdleToRunMotionEnd = false;
	}
	if (bJumpKeyCheck)
	{
		Jump(); 
	}
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		Roll();
	}

	if (bMoveKeyCheck && bSneakKeyCheck)
	{
		Sneak();
	}
	////////////////////////////////////////////
}

void Player::Run()
{
	_CurrentState = Player::State::Run;

	// State
	_RenderComp->Anim(false, true, L"spr_dragon_run", 10, 0.6f);
	Move(_PhysicComp->Dir, 0.f);
	// //
}

void Player::RunState()
{

	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
		return;
	}
	if (!bMoveKeyCheck)
	{
		RunToIdle();
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}	
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		Roll();
	}
	if (bMoveKeyCheck && bSneakKeyCheck)
	{
		Sneak();
	}

	if (CurRunSoundRepeatTime < 0.f)
	{
		CurRunSoundRepeatTime = RunSoundRepeatTime;
		RAND_SOUNDPLAY("player_running", { 1,4 }, 0.6f, false);
	}

	Move(_PhysicComp->Dir, 0.f);


}

void Player::RunToIdle()
{
	_CurrentState = Player::State::Run_to_Idle;

	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]()
	{
		bRunToIdleMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_run_to_idle", 4, 0.2f, std::move(_Notify));
}

void Player::RunToIdleState()
{
	if (bDownKeyCheck && _PhysicComp->bDownLand)
	{
		DownJump();
		return;
	}
	if (bRunToIdleMotionEnd)
	{
		Idle(); 
		bRunToIdleMotionEnd = false;
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}
	if (bDownKeyCheck && !_PhysicComp->bDownLand)
	{
		Roll(); 
	}
	if (bMoveKeyCheck && bSneakKeyCheck)
	{
		Sneak();
	}
}

void Player::Fall()
{
	_PhysicComp->Flying();
	_CurrentState = Player::State::Fall;
	//State 
	_RenderComp->Anim(false, true, L"spr_dragon_fall", 4, 0.2f);
	// // 
};

void Player::FallState()
{
	if (_PhysicComp->bLand&& !_CollisionComp->bDownJump)
	{
		SOUNDPLAY("player_land", 1.f, false);

		Idle(); 
		EffectManager::instance().EffectPush(L"Effect", L"spr_landcloud",
			7, 0.05f, 7 * 0.05f + 0.01f, OBJECT_ID::EID::LAND_CLOUD, true, _PhysicComp->Position + vec3{ 0.f,+15,0.f },
			{ 0,0,0 }, { 3.5,3.5,3.5 });
	}	
	if (bCurWallRideCollision)
	{

		WallRide(); 
	}
	if (bMoveKeyCheck)
	{
		Move(_PhysicComp->Dir, 0.f);
	}
}

void Player::Jump()
{
	_CurrentState = Player::State::Jump;
	
	_PhysicComp->Flying();
	const vec3 Dir{ 0.f,-1.f,0.f };
	SimplePhysics _Physics;
	_Physics.Acceleration = 200.f;
	_Physics.Dir = Dir;
	_Physics.Speed = { 0.f,-850.f ,0.f };
	_Physics.MaxT = 0.3f;
	_PhysicComp->Move(std::move(_Physics));

	RenderComponent::NotifyType _Notify;
	_Notify[4] = [this]()
	{
		bJumpMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_jump", 4, 0.3f, std::move(_Notify));

	EffectManager::instance().EffectPush(L"Effect", L"spr_jumpcloud", 4, 0.05f,
		0.05f * 4.f + 0.01f, OBJECT_ID::EID::JUMP_CLOUD, true,_PhysicComp->Position + vec3{ 0.f,-23.f,0.f },
		{ 0,0,0 }, { 2.5,2.5,2.5 });

	SOUNDPLAY("player_jump", 0.65f, false);

}

void Player::FSM()
{
	switch (_CurrentState)
	{
	case Player::State::Attack:
		AttackState();
		break;
	case Player::State::Crouch:
		CrouchState();
		break;
	case Player::State::Dash:
		DashState();
		break;
	case Player::State::DoorKick:
		DoorKickState();
		break;
	case Player::State::Fall:
		FallState();
		break;
	case Player::State::Flip:
		FlipState();
		break;
	case Player::State::HurtFly:
		HurtFlyState();
		break;
	case Player::State::HurtFly_Begin:
		HurtFlyBeginState();
		break;
	case Player::State::Hurt_Ground:
		HurtGroundState();
		break;
	case Player::State::HurtRecover:
		HurtRecoverState();
		break;
	case Player::State::Idle:
		IdleState();
		break;
	case Player::State::Idle_to_Run:
		IdleToRunState();
		break;
	case Player::State::Idle_to_sneak:
		break;
	case Player::State::Jump:
		JumpState();
		break;
	case Player::State::Post_Crouch:
		PostCrouchState(); 
		break;
	case Player::State::Pre_Crouch:
		PreCrouchState(); 
		break;
	case Player::State::Roll:
		RollState();
		break;
	case Player::State::Run:
		RunState();
		break;
	case Player::State::Run_to_Idle:
		RunToIdleState();
		break;
	case Player::State::Sneak:
		SneakState(); 
		break;
	case Player::State::StairFall:
		break;
	case Player::State::Wall_Ride:
		WallRideState(); 
		break;
	case Player::State::DownJump : 
		DownJumpState();  
		break;
	default:
		break;
	}

	AnyState();
}

void Player::KeyBinding() & noexcept
{
	auto Observer = _This;

	_Anys.emplace_back(
	InputManager::instance().EventRegist([]() 
	{global::bDebug = !global::bDebug;  ShowCursor(global::bDebug); }, 
		'P', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(
		InputManager::instance().EventRegist([]() {RenderManager::instance()._Terrain.bDebugGridRender = 
		!RenderManager::instance()._Terrain.bDebugGridRender; }, 'I', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(
		InputManager::instance().EventRegist([]() {Time::instance().bTimeInfoRender = !Time::instance().bTimeInfoRender; }, 'O', InputManager::EKEY_STATE::DOWN)
	);

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{		
		if (!object::IsValid(Observer))return;
		if (_CurrentState == Player::State::Wall_Ride)return;

		bMoveKeyCheck = true;
		bFrameCurrentCharacterInput = true;
		const vec3 Dir{ 1.f,0.f,0.f };
		
			_PhysicComp->Dir = Dir;
	},
		'D', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{ 	
		if (!object::IsValid(Observer))return;
		if (_CurrentState == Player::State::Wall_Ride)return;


		bMoveKeyCheck = true;
		bFrameCurrentCharacterInput = true;
		const vec3 Dir{ -1.f,0.f,0.f };
			_PhysicComp->Dir = Dir;
	},	'A', InputManager::EKEY_STATE::PRESSING));


	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
		bSneakKeyCheck = true;
	}, VK_CONTROL, InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
		bDownKeyCheck = true;
	}, 'S', InputManager::EKEY_STATE::PRESSING));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
	}, 'S', InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this,  Observer]()
	{ 	
		if (!object::IsValid(Observer))return;
		bFrameCurrentCharacterInput = true;
		bJumpKeyCheck = true;
	},'W', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsReWind() || global::IsReplay())return;

		if (!bInit)
		{
			bInit = true;
			RecordManager::instance().bUpdate = true;
			return;
		}

		bFrameCurrentCharacterInput = true;
		bAttackKeyCheck = true;
		_PhysicComp->Position.z = 0.f;
		vec3 MouseToDistance = global::MousePosWorld - _PhysicComp->Position;
		D3DXVec3Normalize(&_PhysicComp->Dir, &MouseToDistance);
		vec3 Dir = _PhysicComp->Dir;
	}, VK_LBUTTON, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsReWind() || global::IsReplay())return;

		if (global::IsSlow() && CurDashCoolTime<0.f)
		{
			Time::instance().Return();
			bFrameCurrentCharacterInput = true;
			bDashKeyCheck = true;
			bDashRangeRender = false;
			_PhysicComp->Position.z = 0.f;
			vec3 MouseToDistance = global::MousePosWorld - _PhysicComp->Position;
			D3DXVec3Normalize(&_PhysicComp->Dir, &MouseToDistance);
		}
	}, VK_RBUTTON, InputManager::EKEY_STATE::UP));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsReWind() || global::IsReplay())return;
		if (_SpBattery->IsUse() && CurDashCoolTime < 0.f)
		{
			Time::instance().SlowDownTime();
			bFrameCurrentCharacterInput = true;
			bDashRangeRender = true;
			_PhysicComp->Position.z = 0.f;
			vec3 MouseToDistance = global::MousePosWorld - _PhysicComp->Position;
			D3DXVec3Normalize(&_PhysicComp->Dir, &MouseToDistance);
		}
	}, VK_RBUTTON, InputManager::EKEY_STATE::DOWN));


	_Anys.emplace_back(InputManager::instance().EventRegist([this,Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (_SpBattery->IsUse())
		{
			Time::instance().SlowDownTime();
		}
	},
		VK_SHIFT, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (_CurPanitSwitch.expired())return;

		auto _Obj = _CurPanitSwitch.lock();
		auto _PanicSwitch = std::dynamic_pointer_cast<PanicSwitch>(_Obj);
		_PanicSwitch->Interaction();
	},
		VK_SPACE, InputManager::EKEY_STATE::DOWN));


	_Anys.emplace_back(InputManager::instance().EventRegist([this ,Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsSlow())
		{
			Time::instance().Return();
		}
	},
		VK_SHIFT, InputManager::EKEY_STATE::UP));

	
	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (!_SpCurItem)return;

		vec3 MyLocation = _PhysicComp->Position;
		vec3 MouseLocation = global::MousePosWorld;
		vec3 ToMouse = MouseLocation - MyLocation;
		D3DXVec3Normalize(&ToMouse, &ToMouse); 
		_SpCurItem->Throw(MyLocation, ToMouse);
		_SpUIItemIcon->SetIcon(EItem::None);
	},
		'Q', InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsReWind() || global::IsReplay())return;
		RecordManager::instance().ReplayStart();
	},
		VK_F1, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsReWind() || global::IsReplay())return;

		bCheat = !bCheat;
	},
		VK_F2, InputManager::EKEY_STATE::DOWN));

	_Anys.emplace_back(InputManager::instance().EventRegist([this, Observer]()
	{
		if (!object::IsValid(Observer))return;
		if (global::IsReWind() || global::IsReplay())return;

		RecordManager::instance().ReWindStart();
	},
		VK_F3, InputManager::EKEY_STATE::DOWN));
}

void Player::ReWindStart()
{
	vec3 ScreenCenter = global::CameraPos;
	ScreenCenter.x += global::ClientSize.first / 2.f;
	ScreenCenter.y += global::ClientSize.second / 2.f;
	ObjectManager::instance()._Camera.lock()->bUpdate = false;
	ObjectManager::instance().bEnemyUpdate = false;

	RecordManager::instance().bUpdate = false;

	EffectManager::instance().EffectPush(L"Effect", L"NothingBack", 1, 0.2f,
		FLT_MAX, OBJECT_ID::EID::ENONE, false, ScreenCenter, { 0,0,0 }, { 2,2,2 },
		false, false, false, false, 0, 0, 255, false, 0, 0, 0, 0, false, 0, 0);

	EffectManager::instance().EffectPush(L"Effect", L"Nothing", 1, 0.2f,
		FLT_MAX, OBJECT_ID::EID::ENONE, false, ScreenCenter, { 0,0,0 }, { 2,2,2 }, false
		, false, false, false, 0, 0, 255, false, 0, 0, 0, 0, true, 0.4f, 1);

//	SOUNDPLAY("Rewind", 1.f, false);
}

void Player::JumpState()
{
	if (bCurWallRideCollision)
	{
		WallRide(); 
	}
	if (bJumpMotionEnd)
	{
		Fall();
		bJumpMotionEnd = false;
	}
	if (bMoveKeyCheck)
	{
		Move(_PhysicComp->Dir, 0.f);
	}
}

void Player::Attack()
{
	_CurrentState = Player::State::Attack;
	_PhysicComp->Flying();
	RenderComponent::NotifyType _Notify;
	_Notify[7] = [this]() {bAttackMotionEnd = true; };
	_RenderComp->Anim(false, false, L"spr_dragon_attack", 7, 0.20f, std::move(_Notify));
	
	
	_PhysicComp->Position.z = 0.f;
	vec3 Distance = global::MousePosWorld - _PhysicComp->Position;
	vec3 Dir;
	D3DXVec3Normalize(&Dir, &Distance);
	SimplePhysics _Physics;
	_Physics.Acceleration = 100.f;
	_Physics.Dir = Dir;
	_Physics.Speed = Dir * 700.f;
	_Physics.MaxT = 0.25f;
	_Physics.T = 0.f;
	_PhysicComp->Move(std::move(_Physics));
	
	_SpAttackSlash->AttackStart(Dir *10.f, Dir);
	AtAttackDir = Dir;

	_RenderComp->AfterImgOn();
}

void Player::DownJump()
{
	_CurrentState = Player::State::DownJump;
	_PhysicComp->Flying();
	_CollisionComp->bDownJump = true;
	Time::instance().TimerRegist(0.3f,0.3f,0.3f,
		[wpThis = _This, this]()
	{
		if (!object::IsValid(wpThis))return true;
		_CollisionComp->bDownJump = false;
		return false;
	});

	_RenderComp->Anim(false, true, L"spr_dragon_fall", 4, 0.2f);
}
void Player::DownJumpState()
{
	if (!_CollisionComp->bDownJump)
	{
		Fall();
	}
};

void Player::DoorKick()
{
	_CurrentState = Player::State::DoorKick;
	RenderComponent::NotifyType _Notify;
	_Notify[6] = [this]()
	{
		bDoorKickMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_doorkick", 6, 0.3f, std::move(_Notify));

	/*EffectManager::instance().EffectPush(L"Effect",
		L"spr_hit_impact", 5, 0.02f, 0.02f * 5 + 0.01f, OBJECT_ID::HIT_IMPACT, false,
		_PhysicComp->Position + -_CollisionInfo.PushDir * 77,
		{ 0,0,0 }, { 3.3,3.3,0 }, false, false, false, false, 0, 0,
		255, false, 0, ImpactRotZ, 0, 0);
	*/

	float DoorKickRich = 150.f ; 
	if (ToDoorX < 0.f)
	{
		DoorKickRich += 100.f;
	}
	vec3 ToDoor{ ToDoorX,0,0 };
	ToDoor = math::ConvertXAxisDir(ToDoor);
	float RotZ = 0.f;
	if (ToDoor.x < 0.f)
	{
		RotZ = math::PI * 1.f;
	}
	vec3 DoorKickImpactLocation = _PhysicComp->Position + ToDoor *DoorKickRich;
	EffectManager::instance().EffectPush(L"Effect",
		L"spr_hit_impact", 5, 0.05f, 5 * 0.05f + 0.01f, OBJECT_ID::DOOR_KICK_IMPACT, true,
		DoorKickImpactLocation, { 0,0,0 }, { 2.5,2.5,1 }, false, true, false, false, 50, 50, 255, false, 0, RotZ, 0, 0, 0, 0, 0);
	//EffectManager::instance().

	SOUNDPLAY("kickdoor", 1.f, false);
};

void Player::DoorKickState()
{
	if (bDoorKickMotionEnd)
	{
		bDoorKickMotionEnd = false;
		Idle();
	}
}
void Player::PreCrouch()
{
	_CurrentState = Player::State::Pre_Crouch;
	RenderComponent::NotifyType _Notify;
	_Notify[2] = [this]()
	{
		bPreCrouchMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_precrouch",
		2, 0.1f , std::move(_Notify ) );
};

void Player::PreCrouchState()
{
	if (bPreCrouchMotionEnd)
	{
		bPreCrouchMotionEnd = false;
		Crouch();
	}
	if (!bDownKeyCheck)
	{
		Idle();
	}
	if (bMoveKeyCheck)
	{
		IdleToRun();
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}
}
void Player::Crouch()
{
	_CurrentState = Player::State::Crouch;
	_RenderComp->Anim(false, true, L"spr_dragon_crouch", 1, 0.1f);
}
void Player::CrouchState()
{
	if (!bDownKeyCheck)
	{
		PostCrouch();
	}
	if (bMoveKeyCheck)
	{
		Roll();
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}
}
void Player::PostCrouch()
{
	_CurrentState = Player::State::Post_Crouch;
	RenderComponent::NotifyType _Notify;
	_Notify[2] = [this]() 
	{
		bPostCrouchMotionEnd = true; 
	};
	_RenderComp->Anim(false, false, L"spr_dragon_postcrouch", 2, 0.2f,std::move(_Notify));
};

void Player::PostCrouchState()
{
	if (bPostCrouchMotionEnd)
	{
		bPostCrouchMotionEnd = false; 
		Idle(); 
	}
	if (bMoveKeyCheck)
	{
		Roll();
	}
	if (bJumpKeyCheck)
	{
		Jump();
	}
}
void Player::Sneak()
{
	_CurrentState = Player::State::Sneak;
	_RenderComp->Anim(false, true, L"spr_dragon_sneak", 10, 0.8f);
	_RenderComp->PositionCorrection.y += 12;
	bSneak = true;

}
void Player::SneakState()
{
	if (bSneakKeyCheck && !bMoveKeyCheck)
	{
		_RenderComp->PositionCorrection.y -= 12;
		bSneak = false; 
		RunToIdle();
	}
	if (bMoveKeyCheck && !bSneakKeyCheck)
	{
		_RenderComp->PositionCorrection.y -= 12;
		bSneak = false;
		IdleToRun();
	}
	if (bMoveKeyCheck && bSneakKeyCheck && bDownKeyCheck)
	{
		_RenderComp->PositionCorrection.y -= 12;
		bSneak = false;
		Roll();
	}
	if (bJumpKeyCheck)
	{
		_RenderComp->PositionCorrection.y -= 12;
		bSneak = false;
		Jump();
	}

	if (CurFootStepSoundTime < 0.f )
	{
		CurFootStepSoundTime = FootStepSoundTime;
		SOUNDPLAY("footstep", 1.f, false);

	}

	Move(_PhysicComp->Dir, 0.f);
};

void Player::HurtFlyBegin()
{
	_CurrentState = Player::State::HurtFly_Begin;
	RenderComponent::NotifyType _Notify;
	_Notify[2] = [this]()
	{
		bHurtFlyBeginMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_hurtfly_begin", 2, 0.2f, std::move(_Notify));

	bHurt = true;
};

void Player::HurtFlyBeginState()
{
	if (bHurtFlyBeginMotionEnd)
	{
		bHurtFlyBeginMotionEnd = false;
	
		HurtFly();
	}
};

void Player::HurtFly()
{
	_PhysicComp->Flying();
	_CurrentState = Player::State::HurtFly;
	_RenderComp->Anim(false, true, L"spr_dragon_hurtfly", 4, 0.4f);
	if(bFatal)
		bBlooding = true;

}
void Player::HurtFlyState()
{
	// 여기서 땅에닿았다고 판정되면 그라운드 호출 !!
	if (_PhysicComp->bLand)
	{
			bBlooding = false;
			SOUNDPLAY("player_land", 1.f, false);
		HurtGround();
	}
}
void Player::Dash()
{
	_CurrentState = Player::State::Dash;

	_PhysicComp->Flying();
	RenderComponent::NotifyType _Notify;

	_Notify[1] = [this]()
	{
		vec3 Dir;
		if (AtDashDir.x < 0.f)
			Dir = { -1,0,0 };
		else 
			Dir = { +1,0,0 };

		_SpAttackSlash->DashAttackStart(Dir * 80.f, Dir);
	};

	_Notify[2] = [this]() {
		bDashEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_dash", 2, 0.5f, std::move(_Notify));
	_PhysicComp->Position.z = 0.f;

	vec3 Distance = global::MousePosWorld - _PhysicComp->Position;
	vec3 Dir;
	D3DXVec3Normalize(&Dir, &Distance);
	SimplePhysics _Physics;
	_Physics.Acceleration = 0.f;
	_Physics.Dir = Dir;
	_Physics.Speed = (Dir * 9333.f) * (  CurDashRange/ 500.f );
	_Physics.MaxT = 0.04f;
	_Physics.T = 0.f;
	_PhysicComp->Move(std::move(_Physics));

//	_SpAttackSlash->AttackStart(Dir * 30.f, Dir);
	AtDashDir = Dir;
	_RenderComp->AfterImgOn();

	// i 값 조절
	float MaxIndex = 13.f * (CurDashRange / 500.f);

	for (int i = 0; i < MaxIndex; ++i)
	{
		vec3 Location = _PhysicComp->Position;
		float Diff = 40.f;
		EffectManager::instance().EffectPush(L"Dragon", L"spr_dragon_dash", 2, 0.0125f, 0.0251f,
			OBJECT_ID::EID::DRAGON_DASH, false, Location + (Dir * (Diff * i)), { 0,0,0 },
			{ 2.5,2.5,0 }, false, true, false, false, 36, 40, 255, false, 0, atan2f(Dir.y, Dir.x), 0, 0);
	};
}
void Player::DashState()
{
	if (bDashEnd)
	{
		
		bDashEnd = false;
		_RenderComp->AfterImgOff();
		Fall();
	}
}
void Player::HurtGround()
{
	_CurrentState = Player::State::Hurt_Ground;
	RenderComponent::NotifyType _Notify;
	bHurt = true;
	if(bFatal)
		bBloodingOverHead = true;

	_Notify[6] = [this]() 
	{		RAND_SOUNDPLAY("sound_head", { 1,2 }, 0.65f);

		bBloodingOverHead = false;
		bHurtGroundMotionEnd = true; 
	};
	_RenderComp->Anim(false, false, L"spr_dragon_hurtground", 6, 0.5f, std::move(_Notify));
	_RenderComp->PositionCorrection.y += 17 ;

	if (bFatal)
	{
		ReWindStart();
		SOUNDPLAY("sound_head_bloodspurt", 1);
	};
}
void Player::HurtGroundState()
{
	if (bHurtGroundMotionEnd && bAttackKeyCheck)
	{
		// 치명적인 공격을 입은것이라면
		if (bFatal)
		{
			RecordManager::instance().ReWindStart();
			return;
		}

		_RenderComp->PositionCorrection.y -= 17;
		HurtRecover();
		bHurtGroundMotionEnd = false;
	}
};

void Player::HurtRecover()
{
	_CurrentState = Player::State::HurtRecover;
	RenderComponent::NotifyType _Notify;
	_Notify[9] = [this]()
	{
		bHurtRecoverMotionEnd= true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_hurtrecover", 9, 0.7f, move(_Notify));
	_RenderComp->PositionCorrection.y -= 5;
};

void Player::HurtRecoverState()
{
	if (bHurtRecoverMotionEnd)
	{
		_RenderComp->PositionCorrection.y +=5;
		bHurtRecoverMotionEnd = false;
		bHurt = false;
		_PhysicComp->ForceClear();
		InvincibleTime = 1.f;
		SOUNDPLAY("player_land", 1.f, false);


		Idle();
	}
};

void Player::AttackState()
{
	if (bAttackMotionEnd || _SpAttackSlash->bSlashEffectEnd)
	{
		_RenderComp->AfterImgOff();
		bAttackMotionEnd = false;
		Fall();
	}
}

void Player::Roll()
{
	_CurrentState = Player::State::Roll;
	RenderComponent::NotifyType _Notify;
	_Notify[7] = [this]() 
	{
		bRollMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_roll",7, 0.35f ,std::move(_Notify));
	_RenderComp->PositionCorrection.y += 10.f;
	_RenderComp->AfterImgOn();
	SOUNDPLAY("player_roll", 1.f, false);
}

void Player::RollState()
{
	if (bJumpKeyCheck)
	{
		_RenderComp->PositionCorrection.y -= 10.f;
		_RenderComp->AfterImgOff();
		Jump();
	}
	if (bRollMotionEnd)
	{
		_RenderComp->PositionCorrection.y -= 10.f;
		bRollMotionEnd = false;
		_RenderComp->AfterImgOff();
		Idle();
	}

	if (RollEffectDelta < 0.f)
	{
		RollEffectDelta = 0.015f;

		vec3 Pos = _PhysicComp->Position;
		Pos.y += 55.f;
		vec3 Dir;
		D3DXVec3Lerp(&Dir, &-_PhysicComp->Dir, &vec3{ 0,-1.f,0 }, math::Rand<float>({ 0,1 }));
		Pos += Dir*20;
		Dir *= 250;
		EffectManager::instance().EffectPush(L"Effect", L"spr_dustcloud",
			7, 0.04f, 7 * 0.04f + 0.01f, OBJECT_ID::EID::DustCloud, true,
			Pos, Dir, { 2.3,2.3,0 });
	}

	Move(_PhysicComp->Dir, 0.f);
}

void Player::AttackSlash()
{
	// TODO :: 여기서 슬래쉬 객체에게 공격 요청
}

void Player::TimeRegist()
{
	auto Obsever = _This;

	Time::instance().TimerRegist(0.f, 0.f, FLT_MAX,[this,Obsever]() 
	{
		if (Obsever.expired())return true;
		const float Dt =Time::instance().Delta();
		return false;
	});
}

void Player::WallRide()
{
	_CurrentState = Player::State::Wall_Ride;
	_PhysicComp->Dir = FlipDir;
	_PhysicComp->Dir.x *= -1.f;
	_PhysicComp->GravityCoefficient = 0.5f;
	_RenderComp->Anim(false, false, L"spr_dragon_wallride", 2, 0.2f);

	if (WallRideEffectDelta < 0.f)
	{
		float factorX = FlipDir.x < 0.f ? +1 : -1;
		WallRideEffectDelta = 0.05f;
		vec3 Pos = _PhysicComp->Position + vec3{ math::Rand<float>({-1,+1}) * 20.f,50,0 };
		Pos.x += factorX * 20;
		vec3 Dir = math::RotationVec(FlipDir, math::Rand<float>({ -100,100 })) * 150;
		EffectManager::instance().EffectPush(L"Effect",
			L"spr_dustcloud", 7, 0.06f, 7 * 0.06f + 0.01f, OBJECT_ID::EID::DustCloud, true, Pos, Dir, { 2.5,2.5,0 });
	}
}
void Player::WallRideState()
{
	if (bJumpKeyCheck)
	{
		Flip();
		_PhysicComp->GravityCoefficient = 1.f;
		return;
	}
	if (_PhysicComp->bLand)
	{	
		_PhysicComp->GravityCoefficient = 1.f;
		Idle();
		return;
	}
	if (!bCurWallRideCollision)
	{
		Fall();
		return;
	}

	if (CurWallSlideTime < 0.f)
	{
		CurWallSlideTime = WallSlideTime;
		SOUNDPLAY("wallslide", 1.f, false);
	}
	
}
void Player::Flip()
{
	_CurrentState = Player::State::Flip;
	_PhysicComp->Dir.x *= -1.f;
	RenderComponent::NotifyType _Notify;
	_Notify[10] = [this]() {
		bFlipMotionEnd = true;
	};
	_RenderComp->Anim(false, false, L"spr_dragon_flip", 10, 0.4f , std::move(_Notify));
	_PhysicComp->Move(FlipDir * 1500.f, 100.f, 0.4f);
	_RenderComp->AfterImgOn();

	float factor = FlipDir.x < 0.f ? -1 : +1;
	vec3 Dir = FlipDir.x < 0.f ? vec3{ -1.f,0,0 }:vec3{ 1,0,0 };
	Dir *= 55.f;
	float RotZ = math::PI / 2.f * factor;

	EffectManager::instance().EffectPush(L"Effect", L"spr_jumpcloud", 4, 0.05f, 0.05f * 4 + 0.01f,
		OBJECT_ID::EID::JUMP_CLOUD, true, CurWallRideLinePos + Dir, { 0,0,0 }, { 2.5,2.5,2.5 },false
	,false,false,false,0,0,255,false,0, RotZ);
	SOUNDPLAY("player_roll", 1.f, false);

	 // RAND_SOUNDPLAY("wallkick", { 1,3 }, 0.9f, false);


}
void Player::FlipState()
{
	if (bFlipMotionEnd)
	{
		_RenderComp->AfterImgOff();
		bFlipMotionEnd = false;
		Fall();
	}
};

void Player::AnyState()
{
	if (bAttackKeyCheck  && _PhysicComp->bLand && !_CurDoor.expired())
	{
		auto _Door = _CurDoor.lock();
		auto spDoor = std::dynamic_pointer_cast<Door>(_Door);
		vec3 DoorLocationCorrection = spDoor->_TransformComp->Position;
		DoorLocationCorrection.x += Door::XCorrection;

		vec3 ToDoor = DoorLocationCorrection - _TransformComp->Position;
		float Distance = D3DXVec3Length(&ToDoor);
		ToDoorX = ToDoor.x;

		if (Distance < 140 && !spDoor->bOpen && !spDoor->bUse)
		{
			spDoor->Open(ToDoor.x);
			DoorKick();
		}
		else if (bAttackKeyCheck && CurAttackCoolTime < 0.f && !bHurt)
		{
			CurAttackCoolTime = 0.3f;
			Attack();
		}
	}
	else if (bAttackKeyCheck && CurAttackCoolTime<0.f && !bHurt)
	{
		CurAttackCoolTime = 0.3f;
		Attack();
	}

	if (bDashKeyCheck && CurDashCoolTime < 0.f && !bHurt)
	{
		bDashKeyCheck = false;
		CurDashCoolTime = DashCoolTime;
		Dash();
	}
}

void Player::JumpWallRide()
{
	auto _Physic_TransformComp = std::dynamic_pointer_cast<PhysicTransformComponent> (_TransformComp);
	if (!_Physic_TransformComp) return;

	_Physic_TransformComp->Flying();
	bWallJump = true;

	SimplePhysics _Physics;
	_Physics.Acceleration = 200.f;
	_Physics.Dir = FlipDir;
	_Physics.Speed = FlipDir *2000.f;
	_Physics.MaxT = 0.25f;
	_Physics.T = 0.f;

	_Physic_TransformComp->Move(std::move(_Physics));
}

 bool Player::IsEqItem() const& { return _SpUIItemIcon->ItemIcons.second != EItem::None; }

void Player::EqItem(std::shared_ptr<class Item> _Item)
{
	_SpCurItem = _Item;
}
