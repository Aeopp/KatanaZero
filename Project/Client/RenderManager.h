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
	bool bUIRender = true;
private:
	void DebugMouseInfoRender()&noexcept;
};

