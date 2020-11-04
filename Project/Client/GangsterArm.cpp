#include "stdafx.h"
#include "GangsterArm.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "ComponentManager.h"
#include "ObjectManager.h"
#include "Player.h"

OBJECT_ID::EID GangsterArm::GetID()
{
	return OBJECT_ID::EID::ENONE;
}

OBJECT_TAG::ETAG GangsterArm::GetTag()
{
	return OBJECT_TAG::ETAG::EENEMY;
}

std::wstring_view GangsterArm::GetName() const&
{
	return L"GangsterArm"sv;
}

void GangsterArm::Initialize() & noexcept
{
	Super::Initialize();

	_TransformComp = ComponentManager::instance().Insert<TransformComponent>(_This);
	_RenderComp = ComponentManager::instance().Insert<RenderComponent>(_This);

	_TransformComp->Scale *= 1.f;

	_RenderComp->bRender = false;
	_RenderComp->AfterImgOff();
	_RenderComp->PositionCorrection = vec3{ -3,-13,0.f };

	_RenderComp->Anim(true, false, L"spr_arm",
		1, FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0.f, { 1,1 },
		L"Gangster", LAYER::ELAYER::EEFFECT);

	_Target = ObjectManager::instance()._Player.lock();

	_RenderComp->SlowStartColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	_RenderComp->SlowColor = D3DCOLOR_ARGB(255, 125, 125, 125);
	_RenderComp->SlowGoalColor = D3DCOLOR_ARGB(0, 0, 0, 0);
}

void GangsterArm::Update()
{
	vec3 TargetLocation = _Target->_TransformComp->Position;
	vec3 OwnerLocation = _Owner.lock()->_TransformComp->Position;

	vec3 ToTarget = TargetLocation - OwnerLocation;
	D3DXVec3Normalize(&ToTarget, &ToTarget);
	float length = D3DXVec3Length(&ToTarget);

	if (ToTarget.x < 0.f)
	{
		_RenderComp->PositionCorrection = vec3{ 2,-10,0.f };

		//_RenderComp->AnimDir = -1.f;
		_RenderComp->Anim(true, false, L"spr_arm", 2,
			FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f, { 1,1 },
			L"Gangster", LAYER::ELAYER::EEFFECT_OVER);

		_TransformComp->Rotation.z = atan2f(std::abs(ToTarget.y), std::abs(ToTarget.x));
	}
	else
	{
		_RenderComp->PositionCorrection = vec3{ -3,-13,0.f };

		_RenderComp->Anim(true, false, L"spr_arm", 2,
			FLT_MAX, {}, D3DCOLOR_ARGB(255, 255, 255, 255), 0, { 1,1 },
			L"Gangster", LAYER::ELAYER::EEFFECT_OVER);

		_TransformComp->Rotation.z = atan2f(ToTarget.y, ToTarget.x);
	};
}

void GangsterArm::ArmRenderStart()
{
	_RenderComp->bRender = true;

}

void GangsterArm::ArmRenderEnd()
{
	_RenderComp->bRender = false;

}

