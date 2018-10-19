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
	void QueueActionsFromCurrentPlan(ePlanTypes planType);
	void QueueShootActions();
	void QueueRepairActions();
	void QueueHealActions();

	//utility cost calculations
	UtilityInfo GetGatherArrowsUtility();
	UtilityInfo GetGatherLumberUtility();
	UtilityInfo GetGatherBandagesUtility();

	UtilityInfo GetShootUtility();
	UtilityInfo GetRepairUtility();
	UtilityInfo GetHealUtility();

	UtilityInfo GetRepairUtilityPerBuilding(PointOfInterest* poi);
	UtilityInfo GetHealUtilityPerAgent(Agent* agent);

	//utility functions
	float CalculateDistanceUtility(float normalizedDistance);
	float CalculateBuildingHealthUtility(float normalizedBuildingHealth);

	//agent helpers
	float GetAverageAgentHealth();

	//shooting helpers
	IntVector2 GetNearestTileCoordinateOfMapEdgeFromCoordinate(const IntVector2& coordinate);					//O(1)

	//Point of interest helpers
	PointOfInterest* GetNearestPointOfInterestOfTypeFromCoordinate(int poiType, const IntVector2& coodinate);	//O(n)
	PointOfInterest* GetPointOfInterestWithLowestHealth();														//O(?)
	PointOfInterest* GetNearestPointOfInterestWithLowestHealthFromCoordinate(const IntVector2& coordinate);		//O(n+)
	float GetAveragePointOfInterestHealth();

public:
	Map* m_map = nullptr;
	Agent* m_agent = nullptr;
	ePlanTypes m_currentPlan;

private:
	std::stack<ActionData*> m_actionStack;
};

