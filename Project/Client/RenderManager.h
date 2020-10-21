#pragma once
#include "singleton_interface.h"
#include "Terrain.h"

class RenderManager : public singleton_interface<RenderManager>
{
public :
	void Render();
	void LateUpdate();
	void Release()&noexcept;
	void Initialize() & noexcept;
public:
	Terrain _Terrain;
private:
	void DebugMouseInfoRender()&noexcept;
};

