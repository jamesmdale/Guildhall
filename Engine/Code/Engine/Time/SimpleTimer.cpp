#include "Engine\Time\SimpleTimer.hpp"
#include "Engine\Time\Time.hpp"
#include "Engine\Time\Clock.hpp"

//  =============================================================================
SimpleTimer::SimpleTimer()
{

}

//  =============================================================================
SimpleTimer::~SimpleTimer()
{
}

//  =============================================================================
uint64_t SimpleTimer::Start()
{
	m_startTime = GetPerformanceCounter();
	return m_startTime;
}

//  =============================================================================
uint64_t SimpleTimer::Stop()
{
	m_endTime = GetPerformanceCounter();
	return m_endTime;
}

//  =============================================================================
uint64_t SimpleTimer::GetRunningTime()
{
	return m_endTime - m_startTime;
}

//  =============================================================================
void SimpleTimer::Reset()
{
	m_startTime = 0.0;
	m_endTime = 0.0;
}

//  =============================================================================
uint64_t SimpleTimer::GetStartTime()
{
	return m_startTime;
}

//  =============================================================================
uint64_t SimpleTimer::GetStopTime()
{
	return m_endTime;
}
