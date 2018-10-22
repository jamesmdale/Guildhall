#pragma once
#include "Engine\Math\IntVector2.hpp"
#include <stack>

struct ActionData;
class Agent;
class Map;
class PointOfInterest;

enum ePlanTypes
{
	NONE_PLAN_TYPE,
	GATHER_ARROWS_PLAN_TYPE,
	GATHER_LUMBER_PLAN_TYPE,
	GATHER_BANDAGES_PLAN_TYPE,
	SHOOT_PLAN_TYPE,
	REPAIR_PLAN_TYPE,
	HEAL_PLAN_TYPE,
	NUM_PLAN_TYPE
};

struct UtilityInfo
{
	float utility = 0.0f;
	Vector2 endLocation = Vector2::ZERO;
	int targetEntityId = -1;
};

class Planner
{
public:
	Planner(Map* mapReference, Agent* agentReference);
	~Planner();

	//queue management
	void ProcessActionStack(float deltaSeconds);
	void AddActionToStack(ActionData* actionData);
	void ClearStack();

	//planning
	void UpdatePlan();
	void QueueActionsFromCurrentPlan(ePlanTypes planType, const UtilityInfo& info);

	void QueueGatherArrowsAction(const UtilityInfo& info);
	void QueueGatherLumberAction(const UtilityInfo& info);
	void QueueGatherBandagesAction(const UtilityInfo& info);
	void QueueShootActions(const UtilityInfo& info);
	void QueueRepairActions(const UtilityInfo& info);
	void QueueHealActions(const UtilityInfo& info);

	//utility cost calculations
	UtilityInfo GetHighestGatherArrowsUtility();
	UtilityInfo GetHighestGatherLumberUtility();
	UtilityInfo GetHighestGatherBandagesUtility();

	UtilityInfo GetHighestShootUtility();
	UtilityInfo GetHighestRepairUtility();
	UtilityInfo GetHighestHealUtility();

	UtilityInfo GetRepairUtilityPerBuilding(PointOfInterest* poi);
	UtilityInfo GetGatherUitlityPerBuilding(PointOfInterest* poi);
	UtilityInfo GetHealUtilityPerAgent(Agent* agent);

	UtilityInfo GetIdleUtilityInfo();

	void SkewCurrentPlanUtilityValue(UtilityInfo& outInfo);

	//utility functions
	float CalculateDistanceUtility(float normalizedDistance);
	float CalculateBuildingHealthUtility(float normalizedBuildingHealth);
	float CalculateAgentHealthUtility(float normalizedAgentHealth);
	float CalculateAgentGatherUtility(float normalizedGatherUtility);
	float CalculateShootUtility(float normalizedThreatUtility);
	float CalculateIdleUtility();

	//shooting helpers
	IntVector2 GetNearestTileCoordinateOfMapEdgeFromCoordinate(const IntVector2& coordinate);					//O(1)

public:
	Map * m_map = nullptr;
	Agent* m_agent = nullptr;
	ePlanTypes m_currentPlan;

private:
	std::stack<ActionData*> m_actionStack;
};

