#pragma once
#include "Terrain.h"
#include <string_view>

class Scene
{
public:
	virtual ~Scene()noexcept;
public:
	virtual void Render();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Initialize();
	virtual void Release();
public:
	std::vector<std::shared_ptr<class object>>SceneManageObjs;

	

	std::wstring CurrentMapStateKey{};
	std::wstring LoadMapTexInfoFilePath{}; 
	std::wstring LoadCollisionTileInfoFilePath{};
	std::wstring LoadLineInfoFilePath{};
	std::wstring LoadAStarInfoFilePath{};
};


