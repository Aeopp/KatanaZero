#pragma once
#include "object.h"
class Mouse :
    public object
{
public:
	virtual OBJECT_ID::EID   GetID();
	virtual OBJECT_TAG::ETAG GetTag();
	virtual std::wstring_view GetName()const&;
	
	void Initialize() & noexcept override;
	void Update()override;

	std::shared_ptr<class RenderComponent> _RenderComp;
};

