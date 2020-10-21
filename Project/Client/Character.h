#pragma once
#include "object.h"

class Character :
    public object
{
public :
	using Super = object;
public :
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
public :
	virtual void Initialize() & noexcept;

	std::shared_ptr<class RenderComponent> _RenderComp;
	std::shared_ptr<class CollisionComponent> _CollisionComp;
};

