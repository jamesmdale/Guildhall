#include "Engine\Profiler\ProfilerReport.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Time\Time.hpp"

ProfilerReport::~ProfilerReport()
{
	delete(m_root);
	m_root = nullptr;
}

void ProfilerReport::GenerateReportTreeFromFrame(ProfileMeasurement* root)
{
	m_root = new ProfilerReportEntry(root->m_id);
	m_root->PopulateTree(root);
}

void ProfilerReport::GenerateReportFlatFromFrame(ProfileMeasurement* root, eProfilerFlatReportSortMode mode)
{
	m_root = new ProfilerReportEntry(root->m_id);
	m_root->PopulateFlat(root);

	switch (mode)
	{
	case TOTAL_PROFILER_SORT_MODE:
		SortByTotalTime();
		break;
	case SELF_PROFILER_SORT_MODE:
		SortBySelfTime();
		break;
	}
}

void ProfilerReport::PrintReportToDevConsole()
{
	std::vector<std::string>* strings = GetEntriesAsFormattedStrings();

	for (int stringIndex = 0; stringIndex < (int)strings->size(); ++stringIndex)
	{
		DevConsolePrintf(strings->at(stringIndex).c_str());
	}

	delete(strings);
	strings = nullptr;
}

std::vector<std::string>* ProfilerReport::GetEntriesAsFormattedStrings()
{
	GUARANTEE_OR_DIE(m_root != nullptr, "PROFILER REPORT INVALID: NO REPORT TREE GENERATED TO FORMAT AS STRINGS");

	std::vector<std::string>* entryStrings = new std::vector<std::string>();
	entryStrings->push_back("************************************************************************************************************");
	entryStrings->push_back("");
	//recursively get data strings for every node in the tree
	m_root->GetFormattedDataString(entryStrings);
	entryStrings->push_back("");
	entryStrings->push_back("************************************************************************************************************");
	entryStrings->push_back("");

	return entryStrings;
}

void ProfilerReport::SortBySelfTime()
{
	for (int i = 0; i < (int)m_root->m_children.size(); i++)
	{
		for (int j = i + 1; j < (int)m_root->m_children.size(); j++)
		{
			if (m_root->m_children[j]->m_selfTime > m_root->m_children[i]->m_selfTime)
			{
				std::swap(m_root->m_children[j], m_root->m_children[i]);
			}
		}
	}
}

void ProfilerReport::SortByTotalTime()
{
	for (int i = 0; i < (int)m_root->m_children.size(); i++)
	{
		for (int j = i + 1; j < (int)m_root->m_children.size(); j++)
		{
			if (m_root->m_children[j]->m_totalTime > m_root->m_children[i]->m_totalTime)
			{
				std::swap(m_root->m_children[j], m_root->m_children[i]);
			}
		}
	}
}
