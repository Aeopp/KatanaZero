#pragma once
#include "singleton_interface.h"
#include "SceneID.h"
#include "Scene.h"
#include <functional>

class SceneManager : public  singleton_interface<SceneManager>
{
public:
	void Scene_Change(ESceneID _ESceneID);
	void Render();
	void Update();
	void LateUpdate();
	void Initialize();
	void Release();

	ESceneID GetCurrentSceneID()const& { return _CurScene; };

	// SceneType �� Enum ���� ���� ���ν��Ѽ� ������ּ���.
	template<ESceneID TargetID, typename TargetSceneType,
		typename...Params>
		void SceneRegist(Params&&... _Params);
private:
	std::shared_ptr<class Scene> sp_scene{};
	ESceneID _PreScene{ ESceneID::ENone };
	ESceneID _CurScene{ ESceneID::ENone };

	std::map<ESceneID, std::function<std::shared_ptr<Scene>()>> MakeSceneTypeMap;
};

template <ESceneID TargetID, typename TargetSceneType, typename...Params>
void SceneManager::SceneRegist(Params&&... _Params)
{
	static_assert(std::is_base_of_v<Scene, TargetSceneType>, L"Scene Type CompileError");

	MakeSceneTypeMap[TargetID] = [&]() {

		return std::make_shared<TargetSceneType>(std::forward<Params>(_Params)...); };
}



