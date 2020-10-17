#pragma once
#include "singleton_interface.h"

class CollisionManager :
    public singleton_interface<CollisionManager>
{
public :
    void Update();
    void Initialize();
    void Render();
    void LateUpdate();  
private : 
};

