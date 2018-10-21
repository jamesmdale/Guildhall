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
double PerformanceCounterToSeconds(const uint64_t hpc); 
double PerformanceCounterToMilliseconds(const uint64_t hpc);

uint64_t SecondsToPerformanceCounter(const double seconds);

double GetSecondsPerCount();