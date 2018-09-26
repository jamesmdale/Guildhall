#include "Game\EncounterState.hpp"
#include "Game\Map\Map.hpp"
#include "Engine\Time\Clock.hpp"



EncounterState::EncounterState()
{
	m_threatStopWatch = new Stopwatch(GetMasterClock());
	m_threatStopWatch->SetTimer(1.f / m_threatRatePerSecond);
}


EncounterState::~EncounterState()
{
	m_mapReference = nullptr;

	delete(m_threatStopWatch);
	m_threatStopWatch = nullptr;
}

void EncounterState::Update(float deltaSeconds)
{
	//threat update
	if (m_threatStopWatch->HasElapsed())
	{
		m_threat++;
		m_threatStopWatch->Reset();
	}

	/*
		update display averages for each:
		-	average agent health
		-	average building health
		-	average threat per wall
		-	number of agents per goal
		-	num commander allocations
		-	trajectories for agent health, building health, and threat
	*/
}
