#pragma once
#include "RenderComponent.h"
class UIRenderComponent :
    public RenderComponent
{
public :
    void Render()override;
public:
    int32_t Depth{ 0 };
};

