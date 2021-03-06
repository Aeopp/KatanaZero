#pragma once
#include <numeric>
#include <bitset>
#include <Windows.h>
#include <vector >
#include <unordered_map>
#include <array>
#include "DELEGATE.h"
#include "singleton_interface.h"

class InputManager : public singleton_interface<InputManager> 
{
public:
	enum class EKEY_STATE : uint8_t
	{
		BEGIN = 0,
		UP=0,
		DOWN,
		PRESSING,
		END,

		NONE,
	};
private:
	using DelegateType = Delegate<void()>;
	using uint8_t = unsigned char; 
	using VirtualTableKeyDataType = uint8_t/*Window Virtual Key Table Key Size*/;
	static constexpr VirtualTableKeyDataType KeyMappingTableSize = (std::numeric_limits<VirtualTableKeyDataType>::max)();
public:
	void Initialize() & noexcept; 
	void Update() & noexcept;
	void Clear() & noexcept;

	std::shared_ptr<typename InputManager::DelegateType::MyHandleType> 
		EventRegist(typename DelegateType::EventType NotifyEvent,
					typename InputManager::VirtualTableKeyDataType VKKey , 
					typename InputManager::EKEY_STATE _KeyState) & noexcept;
private:
	struct Record
	{
		typename InputManager::VirtualTableKeyDataType _Key;
		typename InputManager::EKEY_STATE _State;
		float T{ 0.f };
	};
	// 리플레이 상황 재현을 위해 플레이어의 입력 저장   마지막 매개변수는 입력한 당시의 시간 T
	std::queue<Record> _InputTable;

	std::bitset<KeyMappingTableSize> EventRegistCheckTable{};
					// 매핑 상태 , 이벤트 목록
	std::unordered_map<VirtualTableKeyDataType,std::pair<EKEY_STATE,std::array<DelegateType,
	(typename std::underlying_type_t<typename InputManager::EKEY_STATE>)EKEY_STATE::END>>>_ActionTable;
};

// 이벤트 등록할때 KeyPressTable 값을 켜주기

inline InputManager::EKEY_STATE operator++(InputManager::EKEY_STATE& _KeyState) {
	_KeyState = static_cast<typename InputManager::EKEY_STATE>((static_cast<typename std::underlying_type_t<typename InputManager::EKEY_STATE>>(_KeyState) + 1));
	return _KeyState;
}