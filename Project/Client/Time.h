#pragma once
#include <chrono>
#include <functional>
#include <vector>
#include "singleton_interface.h"
class Time  : public  singleton_interface<Time>
{
public:
	using NotifyEventType = std::function<bool()>;
	// ������ ���� ���� �Է����ּ���.
	void Initialize(const uint32_t LimitFrame, std::chrono::milliseconds DeltaMax);
	
	void Update();
	void Render();

	float Delta();
	float T();
	void SetDelta(const float DeltaTime)&;
	float TimeScale{ 1.f };
	uint32_t FPS{0};

	void SetT(const float T);

	float a, b, c;
	bool bTimeInfoRender=true;
public:
	// ���� �ð� , �ݺ� �ֱ� , ���� �ð� , �̺�Ʈ
	// ��ȯ�� true �� ���� �ð� ��� ���� �̺�Ʈ ����
	// false �� ��� ����
	void TimerRegist(float initial, float Repeat, float End,
		NotifyEventType _NotifyEvent/*����ǥ���� or std::bind use*/);
public:
	// ��ϵ� �ݹ� �Լ��鿡�� �����ϰ���� Ÿ�ֿ̹� ȣ���Ͽ��ּ���.
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
	// �̺�Ʈ ���ۿ��� , ���� �ð� , �ݺ� �ֱ� , ���� �ð� , ���� ���� �ð�
	std::list<std::tuple<bool,float, float, float, float, float,NotifyEventType>> _NotifyEvents;
private:
	void RenderFPS()const&noexcept;
};

