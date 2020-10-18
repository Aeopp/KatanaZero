#pragma once
#include "singleton_interface.h"
#include "Tags.h"
#include <map>

class ObjectManager : public singleton_interface<ObjectManager>
{
public :
	void Update() ;
	void LateUpdate() ;
	void Initialize() & noexcept;
	void Release() & noexcept;

	template<typename ObjectType,typename...InitParams>
	auto InsertObject(InitParams&&... _Params);
	
	template<typename ObjectType=object>
	auto& FindObject(OBJECT_TAG::ETAG _Tag,OBJECT_ID::EID _ID = OBJECT_ID::EID::ENONE);
private:
	std::map<OBJECT_TAG::ETAG, std::map< OBJECT_ID::EID,std::vector<std::shared_ptr<class object>>>>
	_ObjectMap;
};

template<typename ObjectType, typename ...InitParams>
inline auto ObjectManager::InsertObject(InitParams&&... _Params)
{
	static_assert(std::is_base_of_v<object, ObjectType>,"Type is not a subtype of base");

	auto sp_Object = std::make_shared<ObjectType>();

	sp_Object->_This = sp_Object;

	sp_Object->Initialize(std::forward< InitParams>(_Params)...);

	_ObjectMap[sp_Object->GetTag()][sp_Object->GetID()].emplace_back(sp_Object);

	return sp_Object;
}

template<typename ObjectType>
inline auto& ObjectManager::FindObject(OBJECT_TAG::ETAG _Tag, OBJECT_ID::EID _ID)
{
	const auto& FindTagMap = _ObjectMap[_Tag];

	if(_ID==OBJECT_ID::EID::ENONE)
		return std::vector(std::begin(FindTagMap), std::end(FindTagMap));
	
	const auto& FindIDVec = FindTagMap[_ID];

	if constexpr (std::is_same_v(object,ObjectType))
		return std::vector(std::begin(FindIDVec), std::end(FindIDVec));

	std::vector<std::shared_ptr<ObjectType>> _FindObjectVec;

	std::copy_if(std::begin(FindIDVec),std::end(FindIDVec),std::begin(_FindObjectVec),
	[](const auto& spObject){
		return typeid(std::decay_t<ObjectType>) == typeid(std::decay_t<spObject.element_type>);
	});

	return _FindObjectVec;
}
