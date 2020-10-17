#include "Component.h"

class TransformComponent : public Component
{
public:
	vec3 Scale;
	vec3 Rotation;
	vec3 Position;
	void Update()override;
	void LateUpdate()override;
};
