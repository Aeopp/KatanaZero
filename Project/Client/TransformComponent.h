#pragma once
#include "Component.h"
class TransformComponent :
    public Component
{
public:
	vec3 Scale;
	vec3 Rotation;
	vec3 Position;
	virtual void Update()override;;
	virtual void LateUpdate()override;;
};

