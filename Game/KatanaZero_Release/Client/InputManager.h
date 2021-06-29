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
	// ���÷��� ��Ȳ ������ ���� �÷��̾��� �Է� ����   ������ �Ű������� �Է��� ����� �ð� T
	std::queue<Record> _InputTable;

	std::bitset<KeyMappingTableSize> EventRegistCheckTable{};
					// ���� ���� , �̺�Ʈ ���
	std::unordered_map<VirtualTableKeyDataType,std::pair<EKEY_STATE,std::array<DelegateType,
	(typename std::underlying_type_t<typename InputManager::EKEY_STATE>)EKEY_STATE::END>>>_ActionTable;
};

// �̺�Ʈ ����Ҷ� KeyPressTable ���� ���ֱ�

inline InputManager::EKEY_STATE operator++(InputManager::EKEY_STATE& _KeyState) {
	_KeyState = static_cast<typename InputManager::EKEY_STATE>((static_cast<typename std::underlying_type_t<typename InputManager::EKEY_STATE>>(_KeyState) + 1));
	return _KeyState;
}