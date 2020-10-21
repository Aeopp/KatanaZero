#pragma once
#include <chrono>
#include <functional>
#include <vector>
#include "singleton_interface.h"
class Time  : public  singleton_interface<Time>
{
public:
	using NotifyEventType = std::function<bool()>;
	// 프레임 제한 값을 입력해주세요.
	void Initialize(const uint32_t LimitFrame, std::chrono::milliseconds DeltaMax);
	
	void Update();
	void Render();

	float Delta();
	float T();
	void SetDelta(const float DeltaTime)&;
	float TimeScale{ 1.f };
	uint32_t FPS{0};

	bool bTimeInfoRender=true;
public:
	// 시작 시간 , 반복 주기 , 종료 시간 , 이벤트
	// 반환값 true 시 종료 시간 상관 없이 이벤트 삭제
	// false 시 계속 유지
	void TimerRegist(const float initial, const float Repeat, const float End,
		NotifyEventType _NotifyEvent/*람다표현식 or std::bind use*/);
public:
	// 등록된 콜백 함수들에게 통지하고싶은 타이밍에 호출하여주세요.
	void NotificationCheck()&;
	std::chrono::time_point<std::chrono::high_resolution_clock> PrevTime{};
private:
	std::chrono::duration<float, std::ratio<1, 1000>> SecCheck{};
	std::chrono::duration<float, std::ratio<1, 1000>> LimitDelta{};
	std::chrono::duration<float, std::ratio<1, 1000>> CurrentDelta{};
	std::chrono::duration<float, std::ratio<1, 1000>> Accumulator{};
	std::chrono::steady_clock::time_point CurrentTime{ std::chrono::high_resolution_clock::now() };
	uint32_t _FPSCount{ 0 };
	float DeltaTime{ 0.f };
	float _T{ 0.f };
	std::chrono::milliseconds DeltaMax = 25ms;
	// 이벤트 시작여부 , 시작 시간 , 반복 주기 , 종료 시간 , 현재 측정 시간
	std::list<std::tuple<bool,float, float, float, float, NotifyEventType>> _NotifyEvents;
private:
	void RenderFPS()const&noexcept;
};

