#pragma once
#include "Engine\Core\EngineCommon.hpp"

class SimpleTimer
{
public:

	SimpleTimer();
	~SimpleTimer();

	uint64_t Start();
	uint64_t Stop();
	uint64_t GetRunningTime();
	void Reset();
	uint64_t GetStartTime();
	uint64_t GetStopTime();

private:
	uint64_t m_startTime = 0.0;
	uint64_t m_endTime = 0.0;
};

