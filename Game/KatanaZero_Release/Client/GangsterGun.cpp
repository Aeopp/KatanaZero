#include "stdafx.h"
#include "GangsterGun.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "ObjectManager.h"
#include "Player.h"

OBJECT_ID::EID GangsterGun::GetID()
{
	return OBJECT_ID::EID::GANGSTER_GUN;
}

OBJECT_TAG::ETAG GangsterGun::GetTag()
{
	return OBJECT_TAG::ETAG::EENEMY;
}

std::wstring_view GangsterGun::GetName() const&
{
	return L"GangsterGun"sv;
}

void GangsterGun::Initialize() & noexcept
{
	Super::Initialize();
	
	_TransformComp=ComponentManager::instance().Insert<TransformComponent>(_This);
	_RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);

	_TransformComp->Scale *= 1.f;

	_RenderComp->bRender = false;
	_RenderComp->AfterImgOff();
	_RenderComp->PositionCorrection = vec3{ 18,-13,0.f };

	_RenderComp->Anim(true, false, L"spr_gangstergun",
		1, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, { 1,1 },
		L"Gangster", LAYER::ELAYER::EEFFECT);

	_Target = ObjectManager::instance()._Player.lock();

	_RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	_RenderComp->SlowColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	_RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 0);
	/*_TransformComp->Position = { 0.f,FLT_MAX,0 };*/
}

void GangsterGun::Update()
{
	if (!_Target)return;

	vec3 TargetLocation = _Target->_TransformComp->Position;
	vec3 OwnerLocation = _Owner.lock()->_TransformComp->Position;

	vec3 ToTarget = TargetLocation - OwnerLocation;
	D3DXVec3Normalize(&ToTarget, &ToTarget);
	float length = D3DXVec3Length(&ToTarget);

	if (ToTarget.x < 0.f)
	{
		_RenderComp->PositionCorrection = vec3{ -14,-9,0.f };

		//_RenderComp->AnimDir = -1.f;
		_RenderComp->Anim(true, false, L"spr_gangstergun", 2,
			FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f, { 1,1 },
			L"Gangster", LAYER::ELAYER::EEFFECT);

		_TransformComp->Rotation.z = atan2f( std::abs ( ToTarget.y ) , std::abs(ToTarget.x));
	}
	else
	{
		_RenderComp->PositionCorrection = vec3{ 18,-13,0.f };

		_RenderComp->Anim(true, false, L"spr_gangstergun", 2,
			FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 },
			L"Gangster", LAYER::ELAYER::EEFFECT);

		_TransformComp->Rotation.z = atan2f(ToTarget.y, ToTarget.x);
	};

	//_TransformComp->Rotation.z = std::acosf(std::cosf( ToTarget.x / length ));
}



void GangsterGun::GunRenderStart()
{
	_RenderComp->bRender = true;

}

void GangsterGun::GunRenderEnd()
{
	_RenderComp->bRender = false;
}

void GangsterGun::Attack()
{

}


