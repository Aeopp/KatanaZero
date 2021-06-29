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

	std::shared_ptr<class UIRenderComponent> _RenderComp;
	// 스크린에서의 위치 세팅
	vec3 ScreenPos{ 0.f,0.f,0.f };
};

