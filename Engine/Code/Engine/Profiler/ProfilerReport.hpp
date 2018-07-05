#pragma once
#include "Engine\Profiler\ProfilerReportEntry.hpp"
#include "Engine\Profiler\Profiler.hpp"

#include <string>
#include <map>

class ProfilerReport
{
public:
	ProfilerReport(){};
	~ProfilerReport(){};

	void GenerateReportTreeFromFrame(ProfileMeasurement* root);
	void PrintReportToDevConsole();

	void SortBySelfTime();
	void SortByTotalTime();
	uint64_t GetTotalFrameTime(){ return m_root->m_totalTime; }

public:
	ProfilerReportEntry* m_root = nullptr;
};

