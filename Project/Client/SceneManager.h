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

	ESceneID GetNextSceneID()const& { return _NextScene; }
	ESceneID GetCurrentSceneID()const& { return _CurScene; };
	ESceneID _NextScene{ ESceneID::ENone };
	// SceneType 과 Enum 등의 값을 매핑시켜서 등록해주세요.
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



