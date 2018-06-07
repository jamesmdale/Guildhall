#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Time\Time.hpp"
#include "Engine\Math\MathUtils.hpp"


Stopwatch::Stopwatch(Clock* referenceClock)
{
	SetClock(referenceClock);

	m_startHPC = m_referenceClock->m_total.hpc;
	
}

Stopwatch::~Stopwatch()
{
	if(m_referenceClock != nullptr)
	{
		delete(m_referenceClock);
		m_referenceClock = nullptr;
	}	
}

void Stopwatch::SetClock(Clock* referenceClock)
{
	if (m_referenceClock == nullptr)
	{
		m_referenceClock = GetMasterClock();
	}
	else
	{
		m_referenceClock = referenceClock;
	}
	
}

void Stopwatch::SetTimer(float seconds)
{
	m_intervalHPC = SecondsToPerformanceCounter(seconds);
}

double Stopwatch::GetElapsedTimeInSeconds()
{
	uint64_t elapsedHPC = m_referenceClock->m_total.hpc - m_startHPC;
	
	return PerformanceCounterToSeconds(elapsedHPC);
}

uint64_t Stopwatch::GetElapsedTimeInHPC()
{
	uint64_t elapsedHPC = m_referenceClock->m_total.hpc - m_startHPC;
	return elapsedHPC;
}


double Stopwatch::GetNormalizedElapsedTimeInSeconds()
{
	double normalizedElapsedTime = GetElapsedTimeInSeconds()/PerformanceCounterToSeconds(m_intervalHPC);
	
	return normalizedElapsedTime;
}

bool Stopwatch::HasElapsed()
{
	bool elapsed = false;

	if (GetElapsedTimeInHPC() >= m_intervalHPC)
	{
		elapsed = true;
	}

	return elapsed;
}

void Stopwatch::Reset()
{
	m_startHPC = m_referenceClock->m_total.hpc;
}

bool Stopwatch::CheckAndReset()
{
	bool elapsed = HasElapsed();
	Reset();

	return elapsed;
}

bool Stopwatch::Decrement()
{
	bool elapsed = HasElapsed();

	if (elapsed)
	{
		uint64_t decrementedTime = GetElapsedTimeInHPC() - m_intervalHPC;
		m_startHPC += decrementedTime;	//maintain extra time we've accrued beyond the single interval we are subtracting
	}	

	return elapsed;
}

int Stopwatch::DecrementAll()
{
	bool elapsed = HasElapsed();
	int numIntervalsPassed = 0;

	if (elapsed)
	{	
		double modTime =  Modulus(GetElapsedTimeInSeconds(), PerformanceCounterToSeconds(m_intervalHPC));
		numIntervalsPassed = (int)std::floor(GetNormalizedElapsedTimeInSeconds());
		m_startHPC += (numIntervalsPassed * m_intervalHPC) + (uint64_t)modTime; //maintain extra time we've accrued beyond the intervals we've passed
	}

	return numIntervalsPassed;
}
