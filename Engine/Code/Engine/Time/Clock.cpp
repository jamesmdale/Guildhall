#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <numeric>

//void Clock::GetCurrentTimeSecondsFromTime()
//{
//	m_currentTickTime = GetCurrentTimeSeconds();
//	m_timeSinceClockStart += m_currentTickTime;
//}
//
//float Clock::UpdateTime()
//{
//	GetCurrentTimeSecondsFromTime();
//
//	float deltaSeconds = (float)(m_timeSinceClockStart - m_previousTime);
//	deltaSeconds = ClampFloat(deltaSeconds, 0.0f, 0.1f); //clamp time delta to prevent crazy scenarios happening.
//	m_previousTime = m_timeSinceClockStart;
//
//	return deltaSeconds; 
//}

const double secondsPerPerformanceCount = PerformanceCounterToSeconds(1);
const double minFrameHPC = 1.0/1024.0;
static Clock* g_masterClock = new Clock();
float cumulativeHPC;


Clock::Clock(Clock* parent)
{
	m_parentClock = nullptr;

	m_startHPC = GetPerformanceCounter();
	m_lastFrameHPC = m_startHPC; //set to last frame
	m_timeScale = 1.0;
	m_isPaused = false;

	if(parent != nullptr){
		parent->AddChildClock(this);
	}
}

Clock::~Clock()
{
	m_parentClock = nullptr;
}

void Clock::BeginFrame()
{
	uint64_t currentHPC = GetPerformanceCounter();
	uint64_t elapsedHPC = currentHPC - m_lastFrameHPC;

	double elapsedSeconds = PerformanceCounterToSeconds(elapsedHPC);

	elapsedSeconds = ClampDouble(elapsedSeconds, 0.0, 0.1);

	AdvanceClock(elapsedHPC, elapsedSeconds);
}

void Clock::AdvanceClock(uint64_t elapsedHPC, double elapsedSeconds)
{
	m_frameCount++;

	//clamp elapsedHPC

	if(m_isPaused)
	{
		elapsedHPC = 0;
		elapsedSeconds = 0;
	}
	else
	{
		elapsedHPC = (uint64_t)((double)elapsedHPC * m_timeScale);
		elapsedSeconds = elapsedSeconds * m_timeScale;
	}

	////handle scenarious where the float conversion will be too low to read
	//double hpcSeconds = 0.0;
	//cumulativeHPC += elapsedSeconds;
	//if(cumulativeHPC >= minFrameHPC )
	//{
	//	hpcSeconds = cumulativeHPC;
	//	cumulativeHPC = cumulativeHPC - minFrameHPC;
	//}

	//update frame first
	m_frame.hpcSeconds = elapsedSeconds;
	m_frame.hpc = elapsedHPC;

	m_total.hpcSeconds += m_frame.hpcSeconds;
	m_total.hpc += m_frame.hpc;

	m_lastFrameHPC = m_lastFrameHPC + elapsedHPC;

	for(Clock* iter : m_childrenClocks)
	{
		iter->AdvanceClock(elapsedHPC, elapsedSeconds);
	}
}

void Clock::AddChildClock(Clock* childClock)
{
	childClock->m_parentClock = this;
	m_childrenClocks.push_back(childClock);
}

void Clock::Reset()
{
	m_startHPC = GetPerformanceCounter();
	m_lastFrameHPC = m_startHPC;

	m_frame.hpc = 0;
	m_frame.hpcSeconds = 0;

	m_total.hpc = 0;
	m_total.hpcSeconds = 0;
}

double Clock::GetRunningTime()
{
	uint64_t elapsedHPC = GetMasterClock()->m_total.hpc;

	return PerformanceCounterToSeconds(elapsedHPC);
}

float GetMasterFPS()
{
	return 1.f/GetMasterDeltaSeconds();
}

float GetMasterDeltaSeconds()
{
	if((float)g_masterClock->m_frame.hpcSeconds < std::numeric_limits<float>::min())
	{
		return std::numeric_limits<float>::min();
	}

	float deltaSeconds = ClampFloat((float)g_masterClock->m_frame.hpcSeconds, 0.0f, 0.1f);
	return deltaSeconds;
}

void MasterClockBeginFrame()
{
	g_masterClock->BeginFrame();
}

//double GetCurrentTime()
//{
//	//g_masterClock.
//
//	return 0.0f;
//}

Clock* GetMasterClock()
{
	return g_masterClock;
}