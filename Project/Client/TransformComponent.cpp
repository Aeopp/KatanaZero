#include "stdafx.h"
#include "TransformComponent.h"

matrix TransformComponent::CalcSRTMatrix(const bool bRender)
{
	matrix MScale , MRotation, MTrans;

	D3DXMatrixScaling(&MScale, Scale.x, Scale.y, Scale.z);
	D3DXMatrixRotationYawPitchRoll(&MRotation, Rotation.y, Rotation.x, Rotation.z);
	if(bRender)
		D3DXMatrixTranslation(&MTrans, Position.x-global::CameraPos.x, 
										Position.y-global::CameraPos.y, Position.z);
	else 
		D3DXMatrixTranslation(&MTrans, Position.x , Position.y, Position.z);

	return MScale * MRotation * MTrans;
}

matrix TransformComponent::CalcSRTRMatrix(const bool bRender)
{
	matrix MRevolution;
	D3DXMatrixRotationYawPitchRoll(&MRevolution, Revolution.y, Revolution.x, Revolution.z);
	return CalcSRTMatrix(bRender) * MRevolution;;
}

matrix TransformComponent::CalcWorldMatrix(const bool bRender)
{
	matrix MWorld;
	D3DXMatrixIdentity(&MWorld);

	auto spOwner =_Owner.lock();
	if (!spOwner || !spOwner->_TransformComp)return MWorld;

	auto spOwnerOwner  =spOwner->_Owner.lock();
	if (!spOwnerOwner || !bFollowOwner)	MWorld = CalcSRTRMatrix(bRender);
	else 
		MWorld= CalcSRTRMatrix()* spOwnerOwner->_TransformComp->CalcWorldMatrix(bRender);

	return MWorld;
}

void TransformComponent::Update()
 {
	 Component::Update();
 }

void TransformComponent::LateUpdate()
{
	Component::LateUpdate();

}
