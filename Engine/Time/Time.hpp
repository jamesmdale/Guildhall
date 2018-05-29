//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <stdint.h>

//-----------------------------------------------------------------------------------------------
//double GetCurrentTimeSeconds();

//gets Raw performancecounter
uint64_t GetPerformanceCounter(); 

// converts a performance count the seconds it represents
double PerformanceCounterToSeconds( uint64_t seconds ); 

double GetSecondsPerCount();