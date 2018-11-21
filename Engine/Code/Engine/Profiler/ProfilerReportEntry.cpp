#include "Engine\Profiler\ProfilerReportEntry.hpp"

//  =============================================================================
ProfilerReportEntry::ProfilerReportEntry()
{

}

//  =============================================================================
ProfilerReportEntry::ProfilerReportEntry(const std::string& id)
{
	m_id = id;
}

//  =============================================================================
ProfilerReportEntry::~ProfilerReportEntry()
{
	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
	{
		delete(m_children[childIndex]);
		m_children[childIndex] = nullptr;
	}
	m_children.clear();

	m_parent = nullptr;
}

//  =============================================================================
void ProfilerReportEntry::PopulateTree(ProfileMeasurement* node)
{
	AccumulateData(node);

	for (int childIndex = 0; childIndex < (int)node->m_children.size(); ++childIndex)
	{
		ProfilerReportEntry* entry = GetOrCreateChild(node->m_children[childIndex]->m_id);
		entry->PopulateTree(node->m_children[childIndex]);
		entry = nullptr;
	}

	CompleteData(node);
}

//  =============================================================================
void ProfilerReportEntry::PopulateFlat(ProfileMeasurement* node, ProfilerReportEntry* root)
{
	AccumulateData(node);
	if (root == nullptr)
	{
		root = this;
	}

	for (int childIndex = 0; childIndex < (int)node->m_children.size(); ++childIndex)
	{
		ProfilerReportEntry* entry = root->GetOrCreateChild(node->m_children[childIndex]->m_id);
		entry->PopulateFlat(node->m_children[childIndex], root);

		entry = nullptr;
	}

	CompleteData(node);
}

//  =============================================================================
void ProfilerReportEntry::AccumulateData(ProfileMeasurement* node)
{
	m_callCount++;
	m_totalTime += node->GetElapsedTimeHPC();
}

//  =============================================================================
void ProfilerReportEntry::CompleteData(ProfileMeasurement* node)
{
	uint64_t rootTotalTime = GetRootTotalTimeElapsed(node);

	//total percentage time
	m_totalPercentageOfFrame = (double)((long double)m_totalTime / (long double)rootTotalTime);	

	//self time
	m_selfTime = m_totalTime - GetChildrenTotalTimeElapsed(node);	
	m_selfPercentageOfFrame = (double)((long double)m_selfTime / (long double)rootTotalTime);	
}

//  =============================================================================
ProfilerReportEntry* ProfilerReportEntry::GetOrCreateChild(const std::string& id)
{
	ProfilerReportEntry* entry = FindEntry(id);

	if (entry == nullptr)
	{
		entry = new ProfilerReportEntry();
		entry->m_id = id;
		entry->m_parent = this;
		m_children.push_back(entry);
	}

	return entry;
}

//  =============================================================================
ProfilerReportEntry* ProfilerReportEntry::FindEntry(const std::string& id)
{
	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
	{
		if(m_children[childIndex]->m_id == id)
			return m_children[childIndex];
	}

	return nullptr;
}

//  =============================================================================
uint64_t ProfilerReportEntry::GetChildrenTotalTimeElapsed(ProfileMeasurement* node)
{
	uint64_t totalTime = 0;

	for (int childIndex = 0; childIndex < (int)node->m_children.size(); ++childIndex)
	{
		totalTime += node->m_children[childIndex]->m_endTime - node->m_children[childIndex]->m_startTime;
	}

	return totalTime;
}

//  =============================================================================
uint64_t ProfilerReportEntry::GetRootTotalTimeElapsed(ProfileMeasurement* node)
{
	ProfileMeasurement* parent = node;

	while (parent->m_parent != nullptr)
	{
		parent = parent->m_parent;
	}
	
	uint64_t totalTime = parent->m_endTime - parent->m_startTime;
	parent = nullptr;

	return totalTime;
}

//  =============================================================================
void ProfilerReportEntry::GetFormattedDataString(std::vector<std::string>* entryStrings, int depth)
{
	std::string intentId = "";
	if (depth > 0)
		intentId = "-";

	//std::string rootString = Stringf("%*s %s %-20s %-8s %-3d %-8s %-6.2f %-8s %-6.2f %-8s %-6.2f %-8s %-6.2f", 
	std::string rootString = Stringf("%-*s %-*s %-8s %-3d %-8s %-6.2f %-8s %-8.6f %-8s %-6.2f %-8s %-8.6f", 
		2 * depth,
		"",
		50 - (2 * depth),
		Stringf("%s %s ", "-", m_id.c_str()).c_str(),
		"Call Num:",
		m_callCount,
		"Total_Perc:",
		100.0 * m_totalPercentageOfFrame,
		"Total: ",
		PerformanceCounterToSeconds(m_totalTime),
		"Self_Perc:",
		100.0 * m_selfPercentageOfFrame,
		"Self:",
		PerformanceCounterToSeconds(m_selfTime));

	entryStrings->push_back(rootString);

	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
	{
		ProfilerReportEntry* index = m_children[childIndex];

		//add child data to string
		index->GetFormattedDataString(entryStrings, depth + 1);

		index = nullptr;
	}
}



TODO("Add expand and collapse features");
// Expand and collapse features for later =============================================================================

//void ProfilerReportEntry::CollapseChildren()
//{
//	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
//	{
//		m_children[childIndex]->Collapse();
//	}
//}
//
//void ProfilerReportEntry::Collapse()
//{
//	if(m_parent != nullptr)
//		m_isVisible = false;
//
//	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
//	{
//		m_children[childIndex]->Collapse();
//	}
//}
//
//void ProfilerReportEntry::Expand()
//{
//	m_isVisible = true;
//
//	for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
//	{
//		m_children[childIndex]->Expand();
//	}
//}