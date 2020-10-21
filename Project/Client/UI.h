#pragma once
#include "object.h"

class UI :
    public object
{
public:
	virtual OBJECT_TAG::ETAG GetTag();
public :
	virtual void Initialize() & noexcept;
	virtual void LateInitialize() & noexcept;
	virtual void Release() & noexcept;
	virtual void Update();
	virtual void LateUpdate();

	std::shared_ptr<class RenderComponent> _RenderComp;
};

