#include "Engine\Profiler\ProfilerReport.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Time\Time.hpp"

void ProfilerReport::GenerateReportTreeFromFrame(ProfileMeasurement* root)
{
	m_root = new ProfilerReportEntry(root->m_id);
	m_root->PopulateTree(root);
}

void ProfilerReport::PrintReportToDevConsole()
{
	std::vector<std::string>* entryStrings = new std::vector<std::string>();

	//recursively get data strings for every node in the tree
	m_root->GetFormattedDataString(entryStrings);

	for (int stringIndex = 0; stringIndex < (int)entryStrings->size(); ++stringIndex)
	{
		DevConsolePrintf(entryStrings->at(stringIndex).c_str());
	}
}
