#include "Engine\Profiler\Profiler.hpp"
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
	PROFILER_PUSH();
	ClearStack();

	//set preset to 
	ePlanTypes chosenOutcome = NONE_PLAN_TYPE;
	UtilityInfo highestUtilityInfo = GetIdleUtilityInfo();
	UtilityInfo compareUtilityInfo;

	//utility for gathering arrows
	compareUtilityInfo = GetHighestGatherArrowsUtility();
	if (compareUtilityInfo.utility > highestUtilityInfo.utility)
	{
		highestUtilityInfo = compareUtilityInfo;
		chosenOutcome = GATHER_ARROWS_PLAN_TYPE;
	}
		

	//utility for gathering lumber
	compareUtilityInfo = GetHighestGatherLumberUtility();
	if(compareUtilityInfo.utility > highestUtilityInfo.utility)
	{
		highestUtilityInfo = compareUtilityInfo;
		chosenOutcome = GATHER_LUMBER_PLAN_TYPE;
	}

	//utility for gathering bandages
	compareUtilityInfo = GetHighestGatherBandagesUtility();
	if(compareUtilityInfo.utility > highestUtilityInfo.utility)
	{
		highestUtilityInfo = compareUtilityInfo;
		chosenOutcome = GATHER_BANDAGES_PLAN_TYPE;
	}

	//utility for shooting	
	compareUtilityInfo = GetHighestShootUtility();
	if(compareUtilityInfo.utility > highestUtilityInfo.utility)
	{
		highestUtilityInfo = compareUtilityInfo;
		chosenOutcome = SHOOT_PLAN_TYPE;
	}


	//utility for repairing buildings
	compareUtilityInfo = GetHighestRepairUtility();
	if(compareUtilityInfo.utility > highestUtilityInfo.utility)
	{
		highestUtilityInfo = compareUtilityInfo;
		chosenOutcome = REPAIR_PLAN_TYPE;
	}

	//utility for healing agents
	compareUtilityInfo = GetHighestHealUtility();
	if(compareUtilityInfo.utility > highestUtilityInfo.utility)
	{
		highestUtilityInfo = compareUtilityInfo;
		chosenOutcome = REPAIR_PLAN_TYPE;
	}


	QueueActionsFromCurrentPlan(m_currentPlan, highestUtilityInfo);
}

//  =========================================================================================
void Planner::QueueActionsFromCurrentPlan(ePlanTypes planType, const UtilityInfo& info)
{
	switch (planType)
	{
	case GATHER_ARROWS_PLAN_TYPE:
		QueueGatherArrowsAction(info);
		break;
	case GATHER_LUMBER_PLAN_TYPE:
		QueueGatherLumberAction(info);
		break;
	case GATHER_BANDAGES_PLAN_TYPE:
		QueueGatherBandagesAction(info);
		break;
	case SHOOT_PLAN_TYPE:
		QueueShootActions(info);
		break;
	case REPAIR_PLAN_TYPE:
		QueueRepairActions(info);
		break;
	case HEAL_PLAN_TYPE:
		QueueShootActions(info);
		break;
	default:
		//idle QueueIdleAction(info);
		break;
	}
}

//  =============================================================================
// Queue Actions Functions
//  =============================================================================
void Planner::QueueGatherArrowsAction(const UtilityInfo& info)
{
	ActionData* gatherActionData = new ActionData();
	gatherActionData->m_action = GatherAction;
	gatherActionData->m_finalGoalDestination = info.endLocation;
	gatherActionData->m_interactEntityId = info.targetEntityId;

	m_actionStack.push(gatherActionData);
}

//  =========================================================================================
void Planner::QueueGatherLumberAction(const UtilityInfo& info)
{
	ActionData* gatherActionData = new ActionData();
	gatherActionData->m_action = GatherAction;
	gatherActionData->m_finalGoalDestination = info.endLocation;
	gatherActionData->m_interactEntityId = info.targetEntityId;

	m_actionStack.push(gatherActionData);
}

//  =========================================================================================
void Planner::QueueGatherBandagesAction(const UtilityInfo& info)
{
	ActionData* gatherActionData = new ActionData();
	gatherActionData->m_action = GatherAction;
	gatherActionData->m_finalGoalDestination = info.endLocation;
	gatherActionData->m_interactEntityId = info.targetEntityId;

	m_actionStack.push(gatherActionData);
}

//  =========================================================================================
void Planner::QueueShootActions(const UtilityInfo& info)
{
	ActionData* shootActionData = new ActionData();
	shootActionData->m_action = ShootAction;
	shootActionData->m_finalGoalDestination = info.endLocation;
	shootActionData->m_interactEntityId = info.targetEntityId;

	m_actionStack.push(shootActionData);
}

//  =========================================================================================
void Planner::QueueRepairActions(const UtilityInfo& info)
{
	ActionData* repairActionData = new ActionData();
	repairActionData->m_action = RepairAction;
	repairActionData->m_finalGoalDestination = info.endLocation;
	repairActionData->m_interactEntityId = info.targetEntityId;

	m_actionStack.push(repairActionData);
}

//  =========================================================================================
void Planner::QueueHealActions(const UtilityInfo& info)
{
	ActionData* healActionData = new ActionData();
	healActionData->m_action = HealAction;
	healActionData->m_finalGoalDestination = info.endLocation;
	healActionData->m_interactEntityId = info.targetEntityId;

	m_actionStack.push(healActionData);
}

//  =========================================================================================
// Get utilities
//  =========================================================================================
UtilityInfo Planner::GetHighestGatherArrowsUtility()
{
	UtilityInfo highestGatherArrowsUtility;

	if (m_map->m_armories.size() > 0)
	{
		for (int armoryIndex = 0; armoryIndex < (int)m_map->m_armories.size(); ++armoryIndex)
		{
			UtilityInfo infoForBuilding = GetGatherUitlityPerBuilding(m_map->m_armories[armoryIndex]);
			if (infoForBuilding.utility > highestGatherArrowsUtility.utility)
			{
				highestGatherArrowsUtility = infoForBuilding;
			}
		}
	}

	return highestGatherArrowsUtility;
}

//  =========================================================================================
UtilityInfo Planner::GetHighestGatherLumberUtility()
{
	UtilityInfo highestGatherLumberUtility;

	if (m_map->m_lumberyards.size() > 0)
	{
		for (int lumberyardIndex = 0; lumberyardIndex < (int)m_map->m_lumberyards.size(); ++lumberyardIndex)
		{
			UtilityInfo infoForBuilding = GetGatherUitlityPerBuilding(m_map->m_lumberyards[lumberyardIndex]);
			if (infoForBuilding.utility > highestGatherLumberUtility.utility)
			{
				highestGatherLumberUtility = infoForBuilding;
			}
		}
	}

	return highestGatherLumberUtility;
}

//  =========================================================================================
UtilityInfo Planner::GetHighestGatherBandagesUtility()
{
	UtilityInfo highestGatherBandagesUtility;

	if (m_map->m_medStations.size() > 0)
	{
		for (int medStationIndex = 0; medStationIndex < (int)m_map->m_medStations.size(); ++medStationIndex)
		{
			UtilityInfo infoForBuilding = GetGatherUitlityPerBuilding(m_map->m_medStations[medStationIndex]);
			if (infoForBuilding.utility > highestGatherBandagesUtility.utility)
			{
				highestGatherBandagesUtility = infoForBuilding;
			}
		}
	}

	return highestGatherBandagesUtility;
}

//  =========================================================================================
UtilityInfo Planner::GetHighestShootUtility()
{
	UtilityInfo info;

	if (m_map->m_threat == 0 || m_agent->m_arrowCount == 0)
	{
		return info;
	}

	Vector2 nearestWallPosition = (Vector2)GetNearestTileCoordinateOfMapEdgeFromCoordinate((IntVector2)m_agent->m_position);

	// distance to nearest wall squared ----------------------------------------------
	float distanceToBuildingSquared = GetDistanceSquared(m_agent->m_position, nearestWallPosition);

	//get max distance
	float maxDistanceSquared = GetDistanceSquared(Vector2::ZERO, Vector2(m_map->GetDimensions()));

	//normalize distance
	float normalizedDistance = distanceToBuildingSquared/maxDistanceSquared;

	//apply distance utility formula for utility value
	float distanceUtility = CalculateDistanceUtility(normalizedDistance);


	//normalized threat ----------------------------------------------
	float normalizedThreat = m_map->m_threat/g_maxThreat;

	//apply shoot utility formula for utility value
	float threatUtility = CalculateShootUtility(normalizedThreat);


	//combine distance and health utilities for final utility ----------------------------------------------
	float adjustedShootUtility = distanceUtility * threatUtility;
	info.utility = adjustedShootUtility;

	return info;
}

//  =========================================================================================
UtilityInfo Planner::GetHighestRepairUtility()
{
	UtilityInfo highestRepairUtility = GetRepairUtilityPerBuilding(m_map->m_pointsOfInterest[0]);

	for (int buildingIndex = 1; buildingIndex < (int)m_map->m_pointsOfInterest.size(); ++buildingIndex)
	{
		UtilityInfo utilityInfoForBuilding = GetRepairUtilityPerBuilding(m_map->m_pointsOfInterest[buildingIndex]);
		if (utilityInfoForBuilding.utility > highestRepairUtility.utility)
		{
			highestRepairUtility = utilityInfoForBuilding;
		}
	}

	return highestRepairUtility;
}

//  =========================================================================================
UtilityInfo Planner::GetHighestHealUtility()
{
	UtilityInfo info;
	return info;
}

//  =============================================================================
UtilityInfo Planner::GetRepairUtilityPerBuilding(PointOfInterest* poi)
{
	UtilityInfo info;
	info.utility = 0.f;
	info.endLocation = (Vector2)poi->m_accessCoordinate;
	info.targetEntityId = poi->m_id;

	//easy out if building is at full health
	if (poi->m_health == g_maxHealth || m_agent->m_lumberCount == 0)
	{
		return info;
	}

	// distance to building squared ----------------------------------------------
	float distanceToBuildingSquared = GetDistanceSquared(m_agent->m_position, (Vector2)poi->m_accessCoordinate);
	
	//get max distance
	float maxDistanceSquared = GetDistanceSquared(Vector2::ZERO, Vector2(m_map->GetDimensions()));

	//normalize distance
	float normalizedDistance = distanceToBuildingSquared/maxDistanceSquared;

	//apply distance utility formula for utility value
	float distanceUtility = CalculateDistanceUtility(normalizedDistance);


	//building health ----------------------------------------------
	float normalizedHealth = poi->m_health/g_maxHealth;

	//apply health utility formula for utility value
	float healthUtility = CalculateBuildingHealthUtility(normalizedHealth);


	// combine distance and health utilities for final utility ----------------------------------------------
	float adjustedUtility = distanceUtility * healthUtility;
	info.utility = adjustedUtility;

	return info;
}

//  =============================================================================
UtilityInfo Planner::GetGatherUitlityPerBuilding(PointOfInterest* poi)
{
	UtilityInfo info;
	info.utility = 0.f;
	info.endLocation = (Vector2)poi->m_accessCoordinate;
	info.targetEntityId = poi->m_id;

	int inventoryCountPerType = 0;
	switch (poi->m_type)
	{
	case ARMORY_POI_TYPE:
		inventoryCountPerType = m_agent->m_arrowCount;
		break;
	case LUMBERYARD_POI_TYPE:
		inventoryCountPerType = m_agent->m_lumberCount;
		break;
	case MED_STATION_POI_TYPE:
		inventoryCountPerType = m_agent->m_bandageCount;
		break;
	}

	//easy out if we don't need to gather for this building type
	if (inventoryCountPerType == 0)
	{
		return info;
	}

	// distance to building squared ----------------------------------------------
	float distanceToBuildingSquared = GetDistanceSquared(m_agent->m_position, (Vector2)poi->m_accessCoordinate);

	//get max distance
	float maxDistanceSquared = GetDistanceSquared(Vector2::ZERO, Vector2(m_map->GetDimensions()));

	//normalize distance
	float normalizedDistance = distanceToBuildingSquared/maxDistanceSquared;

	//apply distance utility formula for utility value
	float distanceUtility = CalculateDistanceUtility(normalizedDistance);


	//building health ----------------------------------------------
	float normalizedResourceAmount = inventoryCountPerType/g_maxResourceCarryAmount;

	//apply health utility formula for utility value
	float healthUtility = CalculateAgentGatherUtility(normalizedResourceAmount);


	// combine distance and health utilities for final utility ----------------------------------------------
	float adjustedUtility = distanceUtility * healthUtility;
	info.utility = adjustedUtility;

	return info;
}

//  =========================================================================================
UtilityInfo Planner::GetHealUtilityPerAgent(Agent * agent)
{
	UtilityInfo info;
	return info;
}

//  =============================================================================
UtilityInfo Planner::GetIdleUtilityInfo()
{
	UtilityInfo info;
	info.utility = CalculateIdleUtility();
	return info;
}


//  =========================================================================================
//  Utility Function Calculations
//  =========================================================================================
float Planner::CalculateDistanceUtility(float normalizedDistance)
{
	//  ((1-x)^3 * 0.4f) + 0.1f = y
	float oneMinusNormalizedDistance = 1 - normalizedDistance;
	float utility = ((oneMinusNormalizedDistance * oneMinusNormalizedDistance * oneMinusNormalizedDistance) * 0.4f) + 0.1f;

	return utility;
}

//  =========================================================================================
float Planner::CalculateBuildingHealthUtility(float normalizedBuildingHealth)
{
	//  ((1 - x)^2x * 0.8) = y
	float oneMinusNormalizedBuildingHealth = 1.f - normalizedBuildingHealth;

	float poweredHealth = std::pow(oneMinusNormalizedBuildingHealth, 2.f * normalizedBuildingHealth);
	float utility = poweredHealth * 0.8f;

	return utility;
}

//  =============================================================================
float Planner::CalculateAgentHealthUtility(float normalizedAgentHealth)
{
	//  ((1 - x)^2x * 0.8) = y
	float oneMinusNormalizedAgentHealth= 1.f - normalizedAgentHealth;

	float poweredHealth = std::pow(oneMinusNormalizedAgentHealth, 2.f * normalizedAgentHealth);
	float utility = poweredHealth * 0.8f;

	return utility;
}

//  =============================================================================
float Planner::CalculateAgentGatherUtility(float normalizedResourceCarryAmount)
{
	// ((1-x)^8x * 0.8) = y
	float oneMinusNormalizedGatherUtility = 1.f - normalizedResourceCarryAmount;
	
	float poweredGather = std::pow(oneMinusNormalizedGatherUtility, 8.f * normalizedResourceCarryAmount);
	float utility = poweredGather * 0.8f;

	return utility;
}

//  =============================================================================
float Planner::CalculateShootUtility(float normalizedThreatUtility)
{
	// ((1-(1-x)^2x) * 0.8 = y
	float oneMinusNormalizedThreatUtility = 1.f - normalizedThreatUtility;

	float poweredThreat = 1.f - std::pow(oneMinusNormalizedThreatUtility, 2.f * normalizedThreatUtility);
	float utility = poweredThreat * 0.8f;

	return utility;
}

//  =============================================================================
float Planner::CalculateIdleUtility()
{
	//flat rate for idle.  (very small but will always trump anything of value 0)
	return 0.01f;
}

//  =========================================================================================
//  Helpers
//  =========================================================================================
IntVector2 Planner::GetNearestTileCoordinateOfMapEdgeFromCoordinate(const IntVector2& coordinate)
{
	IntVector2 closestCoordinate = coordinate;

	//maxs of map (walkable is -2).  (assumed mins are 0,0)
	IntVector2 maxTileCoordinates = IntVector2(m_map->m_dimensions.x - 2.f, m_map->m_dimensions.y - 2.f);

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