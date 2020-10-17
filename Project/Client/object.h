#pragma once
#include "Tags.h"
#include <string_view>

class object
{
public:
	bool bDie{ false };
	std::weak_ptr<object> _This{};

	virtual OBJECT_ID::EID   GetID()abstract;
	virtual OBJECT_TAG::ETAG GetTag()abstract;
	virtual std::wstring_view GetName()const& abstract;
public:
	virtual void Initialize()&noexcept;
	virtual void LateInitialize() & noexcept;
	virtual void Release() & noexcept;
	virtual void Update();
	virtual void LateUpdate();
};







