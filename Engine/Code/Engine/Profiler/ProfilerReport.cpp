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

	//recursively get data strings for every node in the tree
	m_root->GetFormattedDataString(entryStrings);

	return entryStrings;
}

