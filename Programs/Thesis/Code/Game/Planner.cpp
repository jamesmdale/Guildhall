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
		QueueRepairActions();
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
	//get coordinate of agent position
	IntVector2 agentCoordinate = m_map->GetTileCoordinateOfPosition(m_agent->m_position);

	if (m_agent->m_lumberCount < g_maxResourceCarryAmount)
	{
		//if we need arrows, we must first gather arrows
		PointOfInterest* gatherPoi = GetNearestPointOfInterestOfTypeFromCoordinate(LUMBERYARD_POI_TYPE, agentCoordinate);

		ActionData* gatherActionData = new ActionData();
		gatherActionData->m_action = GatherAction;
		gatherActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(gatherPoi->m_accessCoordinate);
		gatherActionData->m_interactEntityId = gatherPoi->m_id;

		//after we gather lumber, we will find the nearest/lowest health poi to repair. Then we get the closest wall on that building as our coordinate
		PointOfInterest* finalDestinationPoi = GetNearestPointOfInterestWithLowestHealthFromCoordinate(agentCoordinate);
		IntVector2 closestPoiWallCoordiante = finalDestinationPoi->GetCoordinateBoundsClosestToCoordinate(agentCoordinate);

		ActionData* repairActionData = new ActionData();
		repairActionData->m_action = RepairAction;
		repairActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(closestPoiWallCoordiante);
		repairActionData->m_interactEntityId = finalDestinationPoi->m_id;

		//push both actions onto stack;
		m_actionStack.push(repairActionData);
		m_actionStack.push(gatherActionData);
	}
	else
	{
		//we will find the nearest/lowest health poi to repair. Then we get the closest wall on that building as our coordinate
		PointOfInterest* finalDestinationPoi = GetNearestPointOfInterestWithLowestHealthFromCoordinate(agentCoordinate);
		IntVector2 closestPoiWallCoordiante = finalDestinationPoi->GetCoordinateBoundsClosestToCoordinate(agentCoordinate);		

		ActionData* repairActionData = new ActionData();
		repairActionData->m_action = RepairAction;
		repairActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(closestPoiWallCoordiante);
		repairActionData->m_interactEntityId = finalDestinationPoi->m_id;

		m_actionStack.push(repairActionData);
	}
}

//  =========================================================================================
void Planner::QueueHealActions()
{
	//get coordinate of agent position
	IntVector2 agentCoordinate = m_map->GetTileCoordinateOfPosition(m_agent->m_position);

	if (m_agent->m_bandageCount < g_maxResourceCarryAmount)
	{
		//if we need arrows, we must first gather arrows
		PointOfInterest* gatherPoi = GetNearestPointOfInterestOfTypeFromCoordinate(MED_STATION_POI_TYPE, agentCoordinate);

		ActionData* gatherActionData = new ActionData();
		gatherActionData->m_action = GatherAction;
		gatherActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(gatherPoi->m_accessCoordinate);
		gatherActionData->m_interactEntityId = gatherPoi->m_id;

		//after we gather arrows, we will find the nearest map edge and move there from the access coordinate
		IntVector2 shootDestinationCoordinate = GetNearestTileCoordinateOfMapEdgeFromCoordinate(gatherPoi->m_accessCoordinate);

		ActionData* repairActionData = new ActionData();
		repairActionData->m_action = ShootAction;
		repairActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(shootDestinationCoordinate);
		repairActionData->m_interactEntityId = -1;

		//push both actions onto stack;
		m_actionStack.push(repairActionData);
		m_actionStack.push(gatherActionData);
	}
	else
	{
		PointOfInterest* finalDestinationPoi = GetNearestPointOfInterestWithLowestHealthFromCoordinate(agentCoordinate);
		IntVector2 closestWallCoordiante = finalDestinationPoi->GetCoordinateBoundsClosestToCoordinate(agentCoordinate);

		ActionData* repairActionData = new ActionData();
		repairActionData->m_action = RepairAction;
		repairActionData->m_finalGoalDestination = m_map->GetWorldPositionOfMapCoordinate(closestWallCoordiante);
		repairActionData->m_interactEntityId = finalDestinationPoi->m_id;

		m_actionStack.push(repairActionData);
	}
}

//  =========================================================================================
float Planner::GetShootCost()
{
	/*factors to consider:
		arrowCount
		positionFromNearestWall
		currentThreat
		skill
		bias
	*/

	//if there is no threat, we shouldn't shoot
	if(m_map->m_threat == 0)
		return 9999.f;

	//agent's tile coord to be used throughout calculations
	IntVector2 agentTileCoord = m_map->GetTileCoordinateOfPosition(m_agent->m_position);

	//get arrow capacity efficiency
	float percentageOfArrowCapacity = (float)m_agent->m_arrowCount/(float)g_maxResourceCarryAmount;
	
	//only if we aren't full should we consider distance to armory and gathering costs
	float distanceSquaredToArmorySquared = 0.f;
	IntVector2 startingCoordToWall = agentTileCoord;
	float timeToGatherToFull = 0.f;

	if (percentageOfArrowCapacity < 0.75f)
	{
		PointOfInterest* nearestPoi = GetNearestPointOfInterestOfTypeFromCoordinate(ARMORY_POI_TYPE, agentTileCoord);

		distanceSquaredToArmorySquared = (float)GetDistanceSquared(agentTileCoord, nearestPoi->m_accessCoordinate);	

		timeToGatherToFull = (g_maxResourceCarryAmount - m_agent->m_arrowCount) * (1/g_baseResourceRefillTimePerSecond);
		startingCoordToWall = nearestPoi->m_accessCoordinate;
		percentageOfArrowCapacity = 1.f;

		//cleanup
		nearestPoi = nullptr;
	}
	
	//get distance to nearest wall
	float distanceToNearestWallSquared = (float)GetDistanceSquared(GetNearestTileCoordinateOfMapEdgeFromCoordinate(startingCoordToWall), startingCoordToWall);
	
	//get combatefficiency * performance rate * numArrows = total time
	float timeToDepleteInventory = (g_maxActionPerformanceRatePerSecond * (percentageOfArrowCapacity * g_maxResourceCarryAmount)) / m_agent->m_combatEfficiency;

	//get max calculations for gather and depletion
	float maxTimeToGatherToFull = g_maxResourceCarryAmount / g_baseResourceRefillTimePerSecond;
	float maxTimeToDepleteInventory = (g_maxActionPerformanceRatePerSecond * g_maxResourceCarryAmount) / g_minSkillEfficiency;

	//	threat score graph
	// https://www.desmos.com/calculator/rw09ludgqr

	//combine it all for score.
	float threatScore = ((g_maxThreat - m_map->m_threat + (std::pow( (g_maxThreat - (g_maxThreat * 0.8f)),3)))/std::pow(g_maxThreat,3)) / (1 - m_agent->m_combatBias);														//amount we care about threat
	float totalDistanceSquaredRequirement = (distanceSquaredToArmorySquared + distanceToNearestWallSquared)/g_maxCoordinateDistanceSquared;				//total distance we'd have to travel
	float totalTimeToCompleteTask =  (timeToGatherToFull + timeToDepleteInventory) / (maxTimeToGatherToFull +  maxTimeToDepleteInventory);																		//total time to complete task			

	//take weighted mean
	return (5.f * (1.f - ClampFloat(threatScore, 0.f, 1.f)) + (0.25f * totalDistanceSquaredRequirement) + totalTimeToCompleteTask) / (5.f + 0.25f + 1.f);
}

//  =========================================================================================
float Planner::GetRepairCost()
{
	//if there is no threat, we shouldn't shoot

	float averageHealth = m_map->GetAveragePOIHealth();
	if(averageHealth == (float)g_maxHealth)
		return 9999.f;

	//agent's tile coord to be used throughout calculations
	IntVector2 agentTileCoord = m_map->GetTileCoordinateOfPosition(m_agent->m_position);

	//get arrow capacity efficiency
	float percentOfLumberCapacity = (float)m_agent->m_lumberCount/(float)g_maxResourceCarryAmount;

	//only if we aren't full should we consider distance to armory and gathering costs
	float distanceSquaredToLumberyardSquared = 0.f;
	IntVector2 startingCoordToMostDamagedPOI = agentTileCoord;
	float timeToGatherToFull = 0.f;

	if (percentOfLumberCapacity < 0.75f)
	{
		PointOfInterest* nearestPoi = GetNearestPointOfInterestOfTypeFromCoordinate(LUMBERYARD_POI_TYPE, agentTileCoord);

		distanceSquaredToLumberyardSquared = (float)GetDistanceSquared(agentTileCoord, nearestPoi->m_accessCoordinate);	

		timeToGatherToFull = (g_maxResourceCarryAmount - m_agent->m_arrowCount) * (1/g_baseResourceRefillTimePerSecond);
		startingCoordToMostDamagedPOI = nearestPoi->m_accessCoordinate;
		percentOfLumberCapacity = 1.f;

		//cleanup
		nearestPoi = nullptr;
	}

	//get distance to nearest wall
	float distanceToNearestDamagedPOI = (float)GetDistanceSquared(GetNearestPointOfInterestWithLowestHealthFromCoordinate(startingCoordToMostDamagedPOI)->GetCoordinateBoundsClosestToCoordinate(startingCoordToMostDamagedPOI), startingCoordToMostDamagedPOI);

	//get combatefficiency * performance rate * numArrows = total time
	float timeToDepleteInventory = (g_maxActionPerformanceRatePerSecond * (percentOfLumberCapacity * g_maxResourceCarryAmount)) / m_agent->m_repairEfficiency;

	//get max calculations for gather and depletion
	float maxTimeToGatherToFull = g_maxResourceCarryAmount / g_baseResourceRefillTimePerSecond;
	float maxTimeToDepleteInventory = (g_maxActionPerformanceRatePerSecond * g_maxResourceCarryAmount) / g_minSkillEfficiency;

	//	threat score graph
	// https://www.desmos.com/calculator/rw09ludgqr

	//combine it all for score.
	float poiHealthUrgency = ((g_maxHealth - averageHealth + (std::pow( (g_maxHealth - (g_maxHealth * 0.8f)),3)))/std::pow(g_maxHealth,3)) / (1 - m_agent->m_repairBias);														//amount we care about threat
	float totalDistanceSquaredRequirement = (distanceSquaredToLumberyardSquared + distanceToNearestDamagedPOI)/g_maxCoordinateDistanceSquared;				//total distance we'd have to travel
	float totalTimeToCompleteTask =  (timeToGatherToFull + timeToDepleteInventory) / (maxTimeToGatherToFull +  maxTimeToDepleteInventory);																		//total time to complete task			

																																																				//take weighted mean
	return (5.f * (1.f - ClampFloat(poiHealthUrgency, 0.f, 1.f)) + (0.25f * totalDistanceSquaredRequirement) + totalTimeToCompleteTask) / (5.f + 0.25f + 1.f);
}

//  =========================================================================================
float Planner::GetHealCost()
{
	float cost = 99999.f;

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