#include "stdafx.h"
#include "Component.h"

void Component::Initialize() & noexcept
{

}

void Component::Update()
{
}

void Component::LateUpdate()
{
	auto spOwner = _Owner.lock();
	if (!spOwner) bDie = true;
}

void Component::Render()
{
}
