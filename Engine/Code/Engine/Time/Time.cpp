//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine\Time\Time.hpp"
#include "Engine\Core\WindowsCommon.hpp"


////-----------------------------------------------------------------------------------------------
//double InitializeTime( LARGE_INTEGER& out_initialTime )
//{
//	LARGE_INTEGER countsPerSecond;
//	QueryPerformanceFrequency( &countsPerSecond );
//	QueryPerformanceCounter( &out_initialTime );
//	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
//}
//
//
////-----------------------------------------------------------------------------------------------
//double GetCurrentTimeSeconds()
//{
//	static LARGE_INTEGER initialTime;
//	static double secondsPerCount = InitializeTime( initialTime );
//	LARGE_INTEGER currentCount;
//	QueryPerformanceCounter( &currentCount );
//	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;
//
//	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
//	return currentSeconds;
//}


//this is a new test set of functions
//double GetCurrentTimeSeconds()
//{
//	static LARGE_INTEGER initialTime;
//	LARGE_INTEGER countsPerSecond;
//	QueryPerformanceFrequency( &countsPerSecond );
//	QueryPerformanceCounter( &initialTime );

//	static double countsPerSecond = ( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
//	LARGE_INTEGER currentCount;
//	QueryPerformanceCounter( &currentCount );
//	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;
//
//	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
//	return currentSeconds;
//}

class TimeSystem
{
public:
	TimeSystem()
	{
		LARGE_INTEGER li;
		::QueryPerformanceFrequency(&li);

		//this might need to be the quadpart
		m_frequency = (uint64_t)li.QuadPart;
		m_secondsPerCount = 1.0/(double)m_frequency;
	}
public:
	uint64_t m_frequency;
	double m_secondsPerCount;
};

static TimeSystem g_timeSystem;


uint64_t GetPerformanceCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return (uint64_t)li.QuadPart;
}

// converts a performance count to the seconds it represents
double PerformanceCounterToSeconds( const uint64_t hpc )
{
	return(double)hpc * GetSecondsPerCount();
}

//  =============================================================================
double PerformanceCounterToMilliseconds(const uint64_t hpc)
{
	return(double)hpc * GetSecondsPerCount() * 1000.0;
}

//  =============================================================================
uint SecondsToMilliseconds(float seconds)
{
	return (uint)(seconds * 1000.f);
}

//  =============================================================================
uint MillisecondsToSeconds(float milliseconds)
{
	return (uint)(milliseconds / 1000.f);
}

// converts given seconds to the number of HPCS it represents =============================================================================
uint64_t SecondsToPerformanceCounter(const double seconds)
{
	double secondsPerHPC = PerformanceCounterToSeconds(1);
	double hpcsForSeconds = seconds/secondsPerHPC;

	return (uint64_t)hpcsForSeconds;
}

//  =============================================================================
double GetSecondsPerCount()
{
	return g_timeSystem.m_secondsPerCount;
}

