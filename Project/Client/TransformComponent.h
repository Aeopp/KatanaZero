#pragma once
#include "Component.h"

class TransformComponent :
    public Component
{
public:
	vec3 Scale      = { 1.f,1.f,0.f };
	vec3 Rotation   = { 0.f,0.f,0.f };
	vec3 Position   = { 0.f,0.f,0.f };
	vec3 Revolution = { 0.f,0.f,0.f };

	matrix CalcSRTMatrix(const bool bRender=false);
	matrix CalcSRTRMatrix(const bool bRender = false);
	matrix CalcWorldMatrix(const bool bRender = false);

	virtual void Update()override;;
	virtual void LateUpdate()override;;
};

