#pragma once
#include "object.h"
#include <memory>

class Component
{
public:
	virtual void Initialize()&noexcept;
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	std::weak_ptr<class object> _Owner{};
	bool bDie = false;
private:
	struct Control
	{
		bool bRender = false;
		bool bUpdate = false;
		bool bLateUpdate = false;

		std::function<void(Component& _Comp)> _Render;
		std::function<void(Component& _Comp)> _Update;
		std::function<void(Component& _Comp)> _LateUpdate;
	};
public:
	Control _Control;
};

