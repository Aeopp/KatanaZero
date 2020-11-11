#include "stdafx.h"
#include "TransformComponent.h"

matrix TransformComponent::CalcSRTMatrix(const bool bRender)
{
	matrix MScale, MRotation, MTrans;

	D3DXMatrixScaling(&MScale, Scale.x, Scale.y, Scale.z);
	D3DXMatrixRotationZ(&MRotation, Rotation.z);
	if (bRender)
		D3DXMatrixTranslation(&MTrans, Position.x - global::CameraPos.x,
			Position.y - global::CameraPos.y, Position.z);
	else
		D3DXMatrixTranslation(&MTrans, Position.x, Position.y, Position.z);

	return MScale * MRotation * MTrans;
};

matrix TransformComponent::CalcSRTRMatrix(const bool bRender)
{
	matrix MRevolution;
	D3DXMatrixRotationZ(&MRevolution, Revolution.z);
	return CalcSRTMatrix(bRender) * MRevolution;;
}

matrix TransformComponent::CalcWorldMatrix(const bool bRender)
{
	matrix MWorld;
	D3DXMatrixIdentity(&MWorld);

	auto spOwner = _Owner.lock();
	if (!spOwner || !spOwner->_TransformComp)return MWorld;

	auto spOwnerOwner = spOwner->_Owner.lock();
	if (!spOwnerOwner || !bFollowOwner)	MWorld = CalcSRTRMatrix(bRender);
	else
		MWorld = CalcSRTRMatrix() * spOwnerOwner->_TransformComp->CalcWorldMatrix(bRender);

	return MWorld;
};

matrix TransformComponent::CalcSTMatrix(const bool bRender)
{
	matrix MScale, MTrans;

	D3DXMatrixScaling(&MScale, Scale.x, Scale.y, Scale.z);
	if (bRender)
		D3DXMatrixTranslation(&MTrans, Position.x - global::CameraPos.x,
			Position.y - global::CameraPos.y, Position.z);
	else
		D3DXMatrixTranslation(&MTrans, Position.x, Position.y, Position.z);

	return MScale  * MTrans;
}

matrix TransformComponent::CalcSTWorldMatrix(const bool bRender)
{
	matrix MWorld;
	D3DXMatrixIdentity(&MWorld);

	auto spOwner = _Owner.lock();
	if (!spOwner || !spOwner->_TransformComp)return MWorld;

	auto spOwnerOwner = spOwner->_Owner.lock();
	if (!spOwnerOwner || !bFollowOwner)	MWorld = CalcSTMatrix(bRender);
	else
		MWorld = CalcSTMatrix() * spOwnerOwner->_TransformComp->CalcSTWorldMatrix(bRender);

	return MWorld;
}

void TransformComponent::Update()
 {
	 Component::Update();
 }

void TransformComponent::LateUpdate()
{
	Component::LateUpdate();

	if (isnan(Position.x) || isnan(Position.y))
	{
		Position = PastLocation;
	}

	if (PastLocation == vec3{ 0,0,0 })
	{
		PastLocation = Position;
	}
	else
	{
		PastDir = PastLocation - Position;
		PastLocation = Position;
		D3DXVec3Normalize(&PastDir, &PastDir);
	}


}
