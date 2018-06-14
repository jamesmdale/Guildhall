#pragma once
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"

class Stopwatch
{
public:
	Stopwatch(Clock* referenceClock = nullptr);
	~Stopwatch();

	void SetClock(Clock* referenceClock);
	void SetTimer(float seconds);
	double GetElapsedTimeInSeconds();
	void GetElapsedTimeMinutesAndSecondsOut(float& outMinutes, float& outSeconds);
	double GetNormalizedElapsedTimeInSeconds();
	uint64_t GetElapsedTimeInHPC();
	bool HasElapsed();
	void Reset();
	bool CheckAndReset();
	bool Decrement();
	int DecrementAll();

public:
	Clock* m_referenceClock = nullptr;
	uint64_t m_startHPC;
	uint64_t m_intervalHPC;
};

