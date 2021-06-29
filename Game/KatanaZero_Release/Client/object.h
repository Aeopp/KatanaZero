#pragma once
#include "Tags.h"
#include <string_view>
#include <any>
#include "math.h"

class object
{
public:
	bool bDie{ false };
	bool bOwner{ false };
	std::weak_ptr<object> _This{};
	std::weak_ptr<object> _Owner{};

	std::shared_ptr<class TransformComponent> _TransformComp;

	virtual OBJECT_ID::EID   GetID()abstract;
	virtual OBJECT_TAG::ETAG GetTag()abstract;
	virtual std::wstring_view GetName()const& abstract;
	virtual ~object()noexcept = default;
	template<typename ObjType>
	std::shared_ptr<ObjType>GetThis();

	std::vector<std::any> _Anys;

	void SetOwner(std::weak_ptr<class object> _Owner);
	static bool IsValid(const std::weak_ptr<class object>& _Check);
public:
	virtual void Initialize()&noexcept;
	virtual void LateInitialize() & noexcept;
	virtual void Release() & noexcept;
	virtual void Update();
	virtual void LateUpdate();

	virtual void Hit(std::weak_ptr<class object>_Target,math::Collision::HitInfo _CollisionInfo);
	virtual void MapHit(typename math::Collision::HitInfo _CollisionInfo);

	bool bSmoke = false;
	void LineLanding(const vec3 LineDir);
	void LineOff();
};

// 컴파일 타임 캐스팅이니 타입에 매우 유의
template<typename ObjType>
inline std::shared_ptr<ObjType> object::GetThis()
{
	return std::static_pointer_cast<ObjType>(_This.lock()); 
}
