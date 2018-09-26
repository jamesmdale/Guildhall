#pragma once
#include "Engine\Time\Stopwatch.hpp"

class Map;
class EncounterState
{
public:
	EncounterState();
	~EncounterState();

	void Update(float deltaSeconds);

	//void ChangeThreatRatePerSecond();
	//void ChangeBombardmentRatePerSecond(); (eventually for all sides - north, south, east, west
	//void 

public:

	Map* m_mapReference = nullptr;;

	//threat logic
	float m_threatRatePerSecond = 0;
	float m_threat = 0;
	Stopwatch* m_threatStopWatch = nullptr;

	//float m_bombardmentRatePerSecond = 0;

	//for now all threat comes from one direction
	/*float m_southThreat = 0;
	float m_eastThreat = 0;
	float m_westThreat = 0;*/
};

