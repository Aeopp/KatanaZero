#include "stdafx.h"
#include "math.h"
#include "CollisionComponent.h"
#include "PhysicTransformComponent.h"
#include "GraphicDevice.h"

void CollisionComponent::Render()
{
	if (!global::bDebug)return;

	auto spOwner = _Owner.lock();
	if (!spOwner)return;

	Component::Render();

	constexpr float DebugLineWidth = 2.f;

	matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(true) *
		math::GetCameraJoomMatrix(global::JoomScale,
			vec3{ global::ClientSize.first,global::ClientSize.second,0.f });
	
	auto LocalPoints = math::GetLocalRect(vec2{_CollisionInfo.Width,_CollisionInfo.Height});
	std::array<vec2,std::tuple_size_v<decltype(LocalPoints)>> RenderPoints;

	bool IsRenderable = false;
	
	std::transform(std::begin(LocalPoints), std::end(LocalPoints),
		std::begin(RenderPoints), [&IsRenderable,&MWorld](const vec3& LocalPoint)
	{
		vec2 WorldPoint = vec2{ LocalPoint.x,LocalPoint.y };
		D3DXVec2TransformCoord(&WorldPoint, &WorldPoint, &MWorld);
		IsRenderable |= math::IsPointInnerRect(global::GetScreenRect(), 
			vec3{ WorldPoint.x,WorldPoint.y,0.f });

		return WorldPoint;
	});
	RenderPoints.back() = RenderPoints.front();

	GraphicDevice::instance().GetSprite()->End();
	GraphicDevice::instance().GetLine()->SetWidth(DebugLineWidth);

	if (IsRenderable)
	{
		GraphicDevice::instance().GetLine()->Draw(RenderPoints.data(),
			RenderPoints.size(), _CollisionInfo._Color);
	}

	GraphicDevice::instance().GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
}

void CollisionComponent::Update()
{
	Component::Update();
	
	
}

std::array<vec3, 4ul> CollisionComponent::GetWorldRectPt() & noexcept
{
	auto spOwner = _Owner.lock();
	if (!spOwner)return {};

	matrix MWorld = spOwner->_TransformComp->CalcWorldMatrix(false);
	_CollisionInfo.Pos = spOwner->_TransformComp->Position;

	auto LocalPoints = math::GetLocalRect(vec2{ _CollisionInfo.Width,_CollisionInfo.Height });
	std::array<vec3, 4ul> _WorldRectPt;

	std::transform(
		std::move_iterator(std::begin(LocalPoints)),
		std::move_iterator(std::end(LocalPoints) - 1), 
		std::begin(_WorldRectPt) ,
		[&MWorld](auto&& LocalPt){
			D3DXVec3TransformCoord(&LocalPt, &LocalPt, &MWorld);
			return LocalPt;
		});

	// 이후에 바텀과 탑을 뒤집기
	std::swap(_WorldRectPt[0].y, _WorldRectPt[2].y);
	std::swap(_WorldRectPt[1].y, _WorldRectPt[3].y);

	return _WorldRectPt;
}
