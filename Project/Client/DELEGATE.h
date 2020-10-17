#pragma once
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
using namespace std::placeholders;

template<typename ReturnType, typename... ParamTypes>
struct Delegate_Handler;

template<class Type>class Delegate;

// 함수의 호출은 핸들러의 라이프 타임보다 같거나 짧음
template<typename ReturnType, typename... ParamTypes>
class Delegate<ReturnType(ParamTypes...)>
{
public:
	using MyHandleType = Delegate_Handler<ReturnType, ParamTypes...>;
	using HandleID = int32_t;
	using EventType = std::function<ReturnType(ParamTypes...)>;
private:
	friend class MyHandleType;
private:
	HandleID _ID{ 0 };
	std::map<HandleID, EventType> EventList;
	std::unordered_map<HandleID, std::reference_wrapper<MyHandleType>> _HandleMap;
public:
	~Delegate()noexcept
	{
		Clear();
	}
	// 람다 or Bind(placeholder) 를 사용해주세요.
	[[nodiscard]] std::shared_ptr<MyHandleType> operator+=(EventType _EventRegist)
	{
		EventList.insert(std::make_pair(++_ID, std::move(_EventRegist)));
		auto _Handle = std::make_shared<MyHandleType>(*this, _ID);
		_HandleMap.emplace(_ID, *_Handle);
		return _Handle;
	}
	void operator-=(const HandleID _DeleteEventID)
	{
		EventList.erase(_DeleteEventID);
		_HandleMap.erase(_DeleteEventID);
	}
	[[nodiscard]] auto BroadCast(ParamTypes&&... Params)
	{
#pragma region RETURN_TYPE_IS_VOID
		if constexpr (std::is_same_v<ReturnType, void>)
		{
			for (auto& [ID, NotifyEvent] : EventList)
			{
				auto IsFindIter = _HandleMap.find(ID);
				if (IsFindIter == std::end(_HandleMap))continue;

				auto& HandleRef = IsFindIter->second;

				if (NotifyEvent && !HandleRef.get().IsPause())
					NotifyEvent(std::forward<ParamTypes>(Params)...);
			}
		}
#pragma endregion RETURN_TYPE_IS_VOID
		else
		{
			std::vector<ReturnType> _ReturnVals;

			_ReturnVals.reserve(EventList.size());

			for (auto& [ID, NotifyEvent] : EventList)
			{
				auto IsFindIter = _HandleMap.find(ID);
				if (IsFindIter == std::end(_HandleMap))continue;

				auto& HandleRef = IsFindIter->second;

				if (NotifyEvent && !HandleRef.get().IsPause())
				{
					_ReturnVals.push_back(NotifyEvent(std::forward<ParamTypes>(Params)...));
				}
			}
			return _ReturnVals;
		}
	}
	void Clear()
	{
		EventList.clear();

		for (auto& [_EventID, Handle] : _HandleMap)
		{
			Handle.get().bValid = false;
		}
		_HandleMap.clear();
	}
};

template<typename ReturnType, typename... ParamTypes>
struct Delegate_Handler
{
private:
	using DelegateType = Delegate<ReturnType(ParamTypes...)>;
	friend class DelegateType;
	std::reference_wrapper<DelegateType> _DelegateRef;
	typename DelegateType::HandleID Handle;
	bool bValid{ true };
	bool Pause{ false };
private:
	Delegate_Handler() {};
	friend class std::shared_ptr<Delegate_Handler>;
public:
	bool bFree{ false };

	Delegate_Handler(std::reference_wrapper<DelegateType> _Ref,
		typename DelegateType::HandleID _Handle) :
		_DelegateRef{ _Ref }, Handle{ _Handle }
	{};
	~Delegate_Handler()noexcept
	{
		if(!bFree)
			Invalidate();
	}
	inline void Invalidate()
	{
		if (bValid)
		{
			_DelegateRef.get() -= Handle;
			bValid = false;
		}
	}
	inline void SetPause(const bool _Pause)
	{
		Pause = _Pause;
	}
	inline bool IsPause()
	{
		return Pause;
	}
};
