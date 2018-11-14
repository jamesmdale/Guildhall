#include "Game\SimulationData.hpp"
#include "Game\Definitions\MapDefinition.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"


//  =============================================================================
SimulationData::SimulationData()
{
}

//  =============================================================================
SimulationData::~SimulationData()
{
}

//  =============================================================================
void SimulationData::Initialize(SimulationDefinition* simulationDefinition)
{
	//title cell
	AddCell("Simulation");
	AddNewLine();

	AddCell("SimulationName");
	AddCell(Stringf("%s", simulationDefinition->m_name.c_str()));
	AddNewLine();

	//num agents
	AddCell("NumAgents");
	AddCell(Stringf("%i", simulationDefinition->m_numAgents));
	AddNewLine();

	//num armories
	AddCell("NumArmories");
	AddCell(Stringf("%i", simulationDefinition->m_numArmories));
	AddNewLine();

	//num lumberyards
	AddCell("NumLumberyards");
	AddCell(Stringf("%i", simulationDefinition->m_numLumberyards));
	AddNewLine();

	//num med stations
	AddCell("NumMedStations");
	AddCell(Stringf("%i", simulationDefinition->m_numMedStations));
	AddNewLine();

	//bombardment rate
	AddCell("BombardmentRate");
	AddCell(Stringf("%f", simulationDefinition->m_bombardmentRatePerSecond));
	AddNewLine();

	//threat rate
	AddCell("ThreatRate");
	AddCell(Stringf("%f", simulationDefinition->m_threatRatePerSecond));
	AddNewLine();

	//bombardment starting threat
	AddCell("StartingThreat");
	AddCell(Stringf("%i", simulationDefinition->m_startingThreat));
	AddNewLine();

	//map definition name
	AddCell("MapDimensions");
	AddCell(Stringf("%f,%f", simulationDefinition->m_mapDefinition->m_width, simulationDefinition->m_mapDefinition->m_height));
	AddNewLine();

	//map definitions
	AddCell("MapName");
	AddCell(Stringf("%s", simulationDefinition->m_mapDefinition->m_name.c_str()));
	AddNewLine();
	AddNewLine();

	AddCell("StartingTime");
	AddCell(Stringf("%s", GetCurrentDateTime().c_str()));
	AddNewLine();
}

//  =============================================================================
void SimulationData::WriteEntry(const std::string& entry)
{
	SimulationDataEntry dataEntry = SimulationDataEntry();
	dataEntry.m_value = entry;
	dataEntry.m_timeStamp = GetCurrentDateTime();

	m_entries.push_back(dataEntry);
}

//  =============================================================================
void SimulationData::ExportCSV()
{
	for (int entryIndex = 0; entryIndex < (int)m_entries.size(); ++entryIndex)
	{
		AddCell(Stringf("%s", m_entries[entryIndex].m_value.c_str()));
		AddCell(Stringf("%s", m_entries[entryIndex].m_timeStamp.c_str()));
	}

	std::string newFileName = Stringf("Simulation_%s", GetCurrentDateTime().c_str());

	bool success = WriteToFile(Stringf("%s%s", "Data\\ExportedSimulationData\\", newFileName.c_str()));

	ResetData();
}

//  =============================================================================
void SimulationData::ResetData()
{
	m_entries.clear();
}

