#pragma once
#include "singleton_interface.h"
#include "Component.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"

class ComponentManager :
public singleton_interface<ComponentManager>
{
public :
    void Initialize()&noexcept;
    void Update();
    void LateUpdate();
    void Render();

    // object 가 사용하고 싶은 컴포넌트를 명시해준다고 가정하므로 Self 포인터를 넘겨주세요.
    template<typename ComponentType, typename...InitParams>
    auto Insert(std::weak_ptr<class object> _Owner ,InitParams&&... _Params);

    // 관리하는 컨테이너의 참조를 반환하므로 유의할 것
    template<typename ComponentType>
    auto& Find();

    // 루프를 따로 통제하고 싶은 타입 집합
    std::set<std::type_index> DoesNotUpdateType;
    std::set<std::type_index> DoesNotRenderType;
    std::set<std::type_index> DoesNotLateUpdateType;
private:
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> ComponentMap;
};

template<typename ComponentType, typename ...InitParams>
inline auto ComponentManager::Insert(std::weak_ptr<class object> _Owner ,InitParams && ..._Params)
{
    static_assert(std::is_base_of_v<Component, ComponentType>, "Type is not a subtype of base");
    static_assert(!std::is_same_v<Component, ComponentType>, __FUNCTION__);

    auto sp_Comp = std::make_shared<ComponentType>();
    sp_Comp->_Owner = _Owner;
    sp_Comp->Initialize(std::forward<InitParams>(_Params)...);

    ComponentMap[typeid(ComponentType)].emplace_back(sp_Comp);

    return sp_Comp;
}

template<typename ComponentType>
inline auto& ComponentManager::Find()
{
    static_assert(std::is_base_of_v<Component, ComponentType>, "Type is not a subtype of base");
    static_assert(!std::is_same_v<Component, ComponentType>, __FUNCTION__);
    
                                                           // 해석 방식을 해당 타입의 컴포넌트로 변경
    return reinterpret_cast<std::vector<std::shared_ptr<ComponentType>>&> (ComponentMap[typeid(ComponentType)]);
}

