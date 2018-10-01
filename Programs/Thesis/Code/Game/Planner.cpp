#include "Game\Planner.hpp"
#include "Game\Map\Map.hpp"
#include "Game\Agent.hpp"
#include "Game\PointOfInterest.hpp"
#include "Game\Agent.hpp"
#include "Game\GameCommon.hpp"


//  =========================================================================================
Planner::Planner(Map* mapReference, Agent* agentReference)
{
	m_map = mapReference;
	m_agent = agentReference;
}

//  =========================================================================================
//	Queue Management
//  =========================================================================================
Planner::~Planner()
{
	m_map = nullptr;
}

//  =========================================================================================
void Planner::ProcessActionStack(float deltaSeconds)
{
	if (m_actionStack.size() == 0)
	{
		UpdatePlan();
	}

	//get action at the top of the queue
	if (m_actionStack.size() > 0)
	{
		ActionData* goal = m_actionStack.top();

		//run action
		bool isComplete = goal->m_action(m_agent, goal->m_finalGoalDestination, goal->m_interactEntityId);

		if (isComplete)
		{
			m_actionStack.pop();
		}
	}
}

//  =========================================================================================
void Planner::AddActionToStack(ActionData* actionData)
{
	m_actionStack.push(actionData);
}

//  =========================================================================================
void Planner::ClearStack()
{
	while (m_actionStack.size() > 0)
	{
		m_actionStack.pop();
	}
}

//  =========================================================================================
//  Planning
//  =========================================================================================
void Planner::UpdatePlan()
{
	ClearStack();

	ePlanTypes chosenOutcome = NONE_PLAN_TYPE;
	float lowestCost = 0;

	float shootCost = GetShootCost();
	chosenOutcome = SHOOT_PLAN_TYPE;
	lowestCost = shootCost;

	float repairCost = GetRepairCost();
	if (repairCost < lowestCost)
	{
		chosenOutcome = REPAIR_PLAN_TYPE;
		lowestCost = repairCost;
	}

	float healCost = GetHealCost();
	if (healCost < lowestCost)
	{
		chosenOutcome = HEAL_PLAN_TYPE;
		lowestCost = healCost;
	}

	m_currentPlan = chosenOutcome;

	QueueActionsFromCurrentPlan(m_currentPlan);
}

//  =========================================================================================
void Planner::QueueActionsFromCurrentPlan(ePlanTypes planType)
{
	switch (planType)
	{
	case SHOOT_PLAN_TYPE:
		QueueShootActions();
		break;
	case REPAIR_PLAN_TYPE:
		QueueShootActions();
		break;
	case HEAL_PLAN_TYPE:
		QueueShootActions();
		break;
	}
}

//  =========================================================================================
void Planner::QueueShootActions()
{
	//get coordinate of agent position
	IntVector2 agentCoordinate = m_map->GetTileCoordinateOfPosition(m_agent->m_position);

	if (m_agent->m_arrowCount < g_maxResourceCarryAmount)
	{
		//if we need arrows, we must first gather arrows
		PointOfInterest* gatherPoi = GetNearestPointOfInterestOfTypeFromCoordinate(ARMORY_POI_TYPE, agentCoordinate);

		ActionData* gatherActionData = new ActionData();
		gatherActionData->m_action = GatherAction;
		gatherActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(gatherPoi->m_accessCoordinate);
		gatherActionData->m_interactEntityId = gatherPoi->m_id;

		//after we gather arrows, we will find the nearest map edge and move there from the access coordinate
		IntVector2 shootDestinationCoordinate = GetNearestTileCoordinateOfMapEdgeFromCoordinate(gatherPoi->m_accessCoordinate);
		
		ActionData* shootActionData = new ActionData();
		shootActionData->m_action = ShootAction;
		shootActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(shootDestinationCoordinate);
		shootActionData->m_interactEntityId = -1;

		//push both actions onto stack;
		m_actionStack.push(shootActionData);
		m_actionStack.push(gatherActionData);
	}
	else
	{
		IntVector2 finalDestinationCoordinate = GetNearestTileCoordinateOfMapEdgeFromCoordinate(agentCoordinate);
		ActionData* shootActionData = new ActionData();
		shootActionData->m_action = ShootAction;
		shootActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(finalDestinationCoordinate);
		shootActionData->m_interactEntityId = -1;

		m_actionStack.push(shootActionData);
	}
}

//  =========================================================================================
void Planner::QueueRepairActions()
{
}

//  =========================================================================================
void Planner::QueueHealActions()
{
}

//  =========================================================================================
float Planner::GetShootCost()
{
	float cost = 0.0f;

	return cost;
}

//  =========================================================================================
float Planner::GetRepairCost()
{
	float cost = 0.0f;

	return cost;
}

//  =========================================================================================
float Planner::GetHealCost()
{
	float cost = 0.0f;

	return cost;
}

//  =========================================================================================
//  Helpers
//  =========================================================================================
float Planner::GetAverageAgentHealth()
{
	int numAgents = (int)m_map->m_agents.size();
	int totalHealthOfAllAgents = 0;

	for (int agentIndex = 0; agentIndex < numAgents; ++agentIndex)
	{
		totalHealthOfAllAgents += m_map->m_agents[agentIndex]->m_health;
	}

	return (float)(totalHealthOfAllAgents / numAgents);
}


//  =========================================================================================
PointOfInterest* Planner::GetNearestPointOfInterestOfTypeFromCoordinate(int poiType, const IntVector2& coodinate)
{
	ePointOfInterestType poiTypeCasted = (ePointOfInterestType)poiType;

	int smallestDistanceToPOI = INT_MAX;
	PointOfInterest* closestPoi = nullptr;

	for(int poiIndex = 0; poiIndex < (int)m_map->m_pointsOfInterest.size(); ++poiIndex)
	{
		if(m_map->m_pointsOfInterest[poiIndex]->m_type == poiTypeCasted)
		{
			int distanceSquared = GetDistanceSquared(m_map->m_pointsOfInterest[poiIndex]->m_accessCoordinate, coodinate);
			if(distanceSquared < smallestDistanceToPOI)
			{
				smallestDistanceToPOI = distanceSquared;
				closestPoi = m_map->m_pointsOfInterest[poiIndex];
			}
		}
	}

	return closestPoi;
}

//  =========================================================================================
IntVector2 Planner::GetNearestTileCoordinateOfMapEdgeFromCoordinate(const IntVector2& coordinate)
{
	IntVector2 closestCoordinate = coordinate;

	//maxs of map (walkable is -2).  (assumed mins are 0,0)
	IntVector2 maxTileCoordinates = IntVector2(m_map->m_dimensions.x - 2, m_map->m_dimensions.y - 2);

	/*ex:
	the coordinate (3,6) on a map the size of (8,8) will return (3,7);
	*/

	//find the shortest distance on the X plane
	int distanceFromMaxX = maxTileCoordinates.x - coordinate.x;

	bool isDistanceFromMaxXCloser = false;
	int shortestDistanceX = coordinate.x;

	if(distanceFromMaxX < coordinate.x)
	{
		shortestDistanceX = distanceFromMaxX;
		isDistanceFromMaxXCloser = true;
	}

	//find the shortest distance on the Y plane
	int distanceFromMaxY = maxTileCoordinates.y - coordinate.y;
	bool isDistanceFromMaxYCloser = false;
	int shortestDistanceY = coordinate.y;

	if (distanceFromMaxY < coordinate.y)
	{
		shortestDistanceY = distanceFromMaxY;
		isDistanceFromMaxYCloser = true;
	}

	//compare the shortest distances on each coordinate. The one that is the closest we will move to either min or max
	//(if equal, just choose X)
	if(shortestDistanceY < shortestDistanceX)
	{
		if(isDistanceFromMaxYCloser)
			closestCoordinate.y = maxTileCoordinates.y;
		else
			closestCoordinate.y = 1;
	}
	else
	{
		if (isDistanceFromMaxXCloser)
			closestCoordinate.x = maxTileCoordinates.x;
		else
			closestCoordinate.x = 1;	
	}

	return closestCoordinate;
}

//  =========================================================================================
PointOfInterest* Planner::GetPointOfInterestWithLowestHealth()
{
	PointOfInterest* poiWithLowestHealth = m_map->m_pointsOfInterest[0];

	for (int poiIndex = 1; poiIndex < (int)m_map->m_pointsOfInterest.size(); ++poiIndex)
	{
		if (m_map->m_pointsOfInterest[poiIndex]->m_health < poiWithLowestHealth->m_health)
		{
			poiWithLowestHealth = m_map->m_pointsOfInterest[poiIndex];
		}
	}

	return poiWithLowestHealth;
}

//  =========================================================================================
PointOfInterest* Planner::GetNearestPointOfInterestWithLowestHealthFromCoordinate(const IntVector2& coordinate)
{
	PointOfInterest* poiWithLowestHealth = m_map->m_pointsOfInterest[0];

	for (int poiIndex = 1; poiIndex < (int)m_map->m_pointsOfInterest.size(); ++poiIndex)
	{
		if (m_map->m_pointsOfInterest[poiIndex]->m_health < poiWithLowestHealth->m_health)
		{
			poiWithLowestHealth = m_map->m_pointsOfInterest[poiIndex];
		}
		else if (m_map->m_pointsOfInterest[poiIndex]->m_health == poiWithLowestHealth->m_health)
		{
			int distanceToCurrentPoi = GetDistanceSquared(poiWithLowestHealth->m_accessCoordinate, coordinate);
			int distanceToNewPoi = GetDistanceSquared(m_map->m_pointsOfInterest[poiIndex]->m_accessCoordinate, coordinate);

			if(distanceToNewPoi < distanceToCurrentPoi)
				poiWithLowestHealth = m_map->m_pointsOfInterest[poiIndex];
		}
	}

	return poiWithLowestHealth;
}

//  =========================================================================================
float Planner::GetAveragePointOfInterestHealth()
{
	int numPoi = (int)m_map->m_pointsOfInterest.size();
	int totalPOIHealth = 0;

	for (int poiIndex = 0; poiIndex < numPoi; ++poiIndex)
	{
		totalPOIHealth += m_map->m_pointsOfInterest[poiIndex]->m_health;
	}

	return (float)(totalPOIHealth / numPoi);
}