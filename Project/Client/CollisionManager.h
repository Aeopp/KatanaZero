#pragma once
#include "singleton_interface.h"
#include "CollisionComponent.h"

class CollisionManager :
    public singleton_interface<CollisionManager>
{
public :
    using Tag = typename CollisionComponent::ETag;
public :
    void Update();
    void Initialize();
    void Render();
    void LateUpdate();  

    static constexpr float DistanceCheckMinSq = 20000.f;
    // �ݸ��� �±� ��Ī             Lhs                                    Rhs Set
    std::map<typename CollisionComponent::ETag, std::set<typename CollisionComponent::ETag>> _TagMatchMap
    {
        {Tag::ESmoke_Cloud,{ Tag::EPlayer,Tag::EEnemy,Tag::EPlayerAttack,Tag::EEnemyAttack ,
                            Tag::EItem,Tag::EInteractionObject,Tag::ECollisionEffect} },
        
        {Tag::EPlayer ,{Tag::EDoor,Tag::EEnemy}},
        {Tag::EEnemy,{Tag::EDoor}},
        
        //{Tag::EDoor , {Tag::EPlayer,Tag::EEnemy} } , 
        { Tag::EEnemyAttack, {Tag::EPlayer} },
        {Tag::EPlayerAttack ,{Tag::EEnemy } },
         { Tag::ETrigger,{ Tag::EPlayer} },
       {Tag::EItem  ,{Tag::EEnemy ,Tag::EPlayer} },
    };
private : 
};

