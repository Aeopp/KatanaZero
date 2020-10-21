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
};

