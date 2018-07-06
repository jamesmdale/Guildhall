#pragma once
#include "Engine\Profiler\ProfilerReportEntry.hpp"
#include "Engine\Profiler\Profiler.hpp"

#include <string>
#include <map>

enum eProfilerFlatReportSortMode
{
	SELF_PROFILER_SORT_MODE,
	TOTAL_PROFILER_SORT_MODE,
	NUM_PROFILER_SORT_MODES
};

class ProfilerReport
{
public:
	ProfilerReport(){};
	~ProfilerReport();

	void GenerateReportTreeFromFrame(ProfileMeasurement* root);
	void GenerateReportFlatFromFrame(ProfileMeasurement* root, eProfilerFlatReportSortMode mode = TOTAL_PROFILER_SORT_MODE);
	void PrintReportToDevConsole();

	std::vector<std::string>* GetEntriesAsFormattedStrings();

	void SortBySelfTime();
	void SortByTotalTime();
	uint64_t GetTotalFrameTime(){ return m_root->m_totalTime; }

public:
	ProfilerReportEntry* m_root = nullptr;
};

void Report(Command& cmd);


