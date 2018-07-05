#pragma once
#include "Engine\Profiler\Profiler.hpp"
#include <string>
#include <map>
#include <vector>

class ProfilerReportEntry
{
public:
	ProfilerReportEntry();
	ProfilerReportEntry(const std::string& id);
	~ProfilerReportEntry();

	void PopulateTree(ProfileMeasurement* node);
	void AccumulateData(ProfileMeasurement* node);
	void CompleteData(ProfileMeasurement * node);
	void PopulateFlat(ProfileMeasurement* node);

	ProfilerReportEntry* GetOrCreateChild(const std::string& id);
	ProfilerReportEntry* FindEntry(const std::string& id);
	uint64_t GetRootTotalTimeElapsed(ProfileMeasurement* node);
	uint64_t GetChildrenTotalTimeElapsed(ProfileMeasurement* node);

	void GetFormattedDataString(std::vector<std::string>* entryStrings);

public:
	bool m_isVisible = true;

	std::string m_id = "";
	int m_callCount = 0;

	//total time
	double m_totalPercentageOfFrame = 0.0;
	uint64_t m_totalTime = 0.f;

	//self time
	double m_selfPercentageOfFrame = 0.0;
	uint64_t m_selfTime = 0.f;	

	std::map<std::string, ProfilerReportEntry*> m_children;
	ProfilerReportEntry* m_parent = nullptr;	
};