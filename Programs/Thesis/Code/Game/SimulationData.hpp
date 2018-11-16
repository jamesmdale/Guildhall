#pragma once
#include <vector>
#include <string>
#include "Engine\File\CSVWriter.hpp"
#include "Game\Definitions\SimulationDefinition.hpp"


struct SimulationDataEntry
{
	std::string m_value = "";
	std::string m_timeStamp = "";	
};

class SimulationData : CSVWriter
{
public:
	SimulationData();
	~SimulationData();

	void Initialize(SimulationDefinition* simulationDefinition);

	void WriteEntry(const std::string& entry);
	void ExportCSV();
	void ResetData();

public:
	std::vector<SimulationDataEntry> m_entries;
	SimulationDefinition* m_simulationDefinitionReference = nullptr;
};

