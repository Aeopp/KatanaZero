#pragma once
#include "singleton_interface.h"
#include "Component.h"
#include "TransformComponent.h"
#include "PhysicTransformComponent.h"
#include "CollisionComponent.h"

class ComponentManager :
public singleton_interface<ComponentManager>
{
public :
    void Initialize()&noexcept;
    void Update();
    void LateUpdate();
    void Render();

    // object �� ����ϰ� ���� ������Ʈ�� ������شٰ� �����ϹǷ� Self �����͸� �Ѱ��ּ���.
    template<typename ComponentType, typename...InitParams>
    auto Insert(std::weak_ptr<class object> _Owner ,InitParams&&... _Params);

    // �����ϴ� �����̳��� ������ ��ȯ�ϹǷ� ������ ��
    template<typename ComponentType>
    auto& Find();

    // ������ ���� �����ϰ� ���� Ÿ�� ����
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
    
                                                           // �ؼ� ����� �ش� Ÿ���� ������Ʈ�� ����
    return reinterpret_cast<std::vector<std::shared_ptr<ComponentType>>&> (ComponentMap[typeid(ComponentType)]);
}

