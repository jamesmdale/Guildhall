#include "Game\Agent.hpp"
#include "Game\Game.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Map\Map.hpp"
#include "Game\GameCommon.hpp"
#include "Game\PointOfInterest.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Core\Transform2D.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Utility\AStar.hpp"


//  =========================================================================================
Agent::Agent()
{
}

//  =========================================================================================

Agent::Agent(Vector2 startingPosition, IsoSpriteAnimSet* animationSet, Map* mapReference)
{
	m_position = startingPosition;
	m_animationSet = animationSet;
	m_mapReference = mapReference;

	m_intermediateGoalPosition = m_position;

	m_animationSet->SetCurrentAnim("idle");

	m_id = mapReference->m_agents.size();
}

//  =========================================================================================

Agent::~Agent()
{
	m_mapReference = nullptr;
}

//  =========================================================================================
void Agent::Update(float deltaSeconds)
{
	ProcessActionStack(deltaSeconds);

	UpdateSpriteRenderDirection();
	m_animationSet->Update(deltaSeconds);
}

//  =========================================================================================
void Agent::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Sprite sprite = *m_animationSet->GetCurrentSprite(m_spriteDirection);

	float dimensions = Window::GetInstance()->GetClientWidth() * g_tilePercentageOfWindow;

	Texture* texture = sprite.GetSpriteTexture();
	Vector2 spritePivot = sprite.m_definition->m_pivot;

	IntVector2 spriteDimensions = sprite.GetDimensions();

	AABB2 bounds;
	bounds.mins.x = 0.f - (spritePivot.x) * dimensions;
	bounds.maxs.x = bounds.mins.x + 1.f * dimensions;
	bounds.mins.y = 0.f - (spritePivot.y) * dimensions;
	bounds.maxs.y = bounds.mins.y + 1.f * dimensions;

	theRenderer->SetShader(theRenderer->CreateOrGetShader("agents"));
	theRenderer->DrawTexturedAABB(m_transform.GetWorldMatrix(), bounds, *theRenderer->CreateOrGetTexture(sprite.m_definition->m_diffuseSource), Vector2(sprite.GetNormalizedUV().mins.x, sprite.GetNormalizedUV().maxs.y), Vector2(sprite.GetNormalizedUV().maxs.x, sprite.GetNormalizedUV().mins.y), Rgba::WHITE);
	theRenderer->SetShader(theRenderer->CreateOrGetShader("default"));

	theRenderer = nullptr;
}

//  =========================================================================================
bool Agent::GetPathToDestination(const Vector2& goalDestination)
{
	m_currentPath = std::vector<Vector2>(); //clear vector

	IntVector2 startCoord = m_mapReference->GetTileCoordinateOfPosition(m_position);
	IntVector2 endCoord = m_mapReference->GetTileCoordinateOfPosition(goalDestination);
	bool isDestinationFound = false;

	m_currentPath.push_back(goalDestination);

	//add the location
	isDestinationFound = AStarSearch(m_currentPath, startCoord, endCoord, m_mapReference);

	return isDestinationFound;
}

//  =========================================================================================
bool Agent::GetIsAtPosition(const Vector2 & goalDestination)
{
	if (m_mapReference->GetTileCoordinateOfPosition(m_position) != m_mapReference->GetTileCoordinateOfPosition(goalDestination))
		return false;
	else
		return true;
}

//  =========================================================================================
void Agent::ProcessActionStack(float deltaSeconds)
{
	//get action at the top of the queue
	if (m_actionStack.size() > 0)
	{
		ActionData* goal = m_actionStack.top();

		//run action
		bool isComplete = goal->m_action(this, goal->m_finalGoalDestination, goal->m_interactEntityId);

		if (isComplete)
		{
			m_actionStack.pop();
		}			
	}	
}

//  =========================================================================================
void Agent::AddActionToStack(ActionData* actionData)
{
	m_actionStack.push(actionData);
}

//  =========================================================================================
void Agent::ClearStack()
{
	while (m_actionStack.size() > 0)
	{
		m_actionStack.pop();
	}
}

//  =========================================================================================
void Agent::UpdateSpriteRenderDirection()
{
	//calculate the largest dot between facing or turned away
	float dotValue = 0;
	float northDot = DotProduct(Vector2(MAP_NORTH), Vector2(m_forward));
	float southDot = DotProduct(Vector2(MAP_SOUTH), Vector2(m_forward));
	float eastDot = DotProduct(Vector2(MAP_EAST), Vector2(m_forward));
	float westDot = DotProduct(Vector2(MAP_WEST), Vector2(m_forward));

	IntVector2 direction;

	//set the direction we are most aligned with between north and south
	float highestLongitudeDot = 0;
	if (northDot > southDot)
	{
		direction.y = 1;
		highestLongitudeDot = northDot;
	}
	else
	{
		direction.y = -1;
		highestLongitudeDot = southDot;
	}
	
	//set the direction we are most aligned with between east and west
	float highestLatitudeDot = 0;
	if (eastDot > westDot)
	{
		direction.x = 1;
		highestLatitudeDot = eastDot;
	}
	else
	{
		direction.x = -1;
		highestLatitudeDot = westDot;
	}

	//compare north and south dot to the highest of the east and west dot. Whichever is higher is the direction we are most facing.
	if (highestLongitudeDot > highestLatitudeDot)
		direction.x = 0;
	else
		direction.y = 0;

	//set the final direction.
	m_spriteDirection = direction;
}




//  =========================================================================================
//  Actions
//  =========================================================================================

bool MoveAction(Agent* agent, const Vector2& goalDestination, int interactEntityId)
{
	UNUSED(interactEntityId);

	bool isComplete = false;

	TODO("might optimize not doing this every frame later");
	agent->m_animationSet->SetCurrentAnim("walk");

	// early outs ----------------------------------------------
	if (agent->m_mapReference->GetTileCoordinateOfPosition(agent->m_position) == agent->m_mapReference->GetTileCoordinateOfPosition(goalDestination))
		return true;

	//if we don't have a path to the destination, get a new path
	if (agent->m_currentPath.size() == 0)
		agent->GetPathToDestination(goalDestination);


	//We have a path, follow it.
	if (agent->m_mapReference->GetTileCoordinateOfPosition(agent->m_position) != agent->m_mapReference->GetTileCoordinateOfPosition(agent->m_currentPath.at(agent->m_currentPath.size() - 1)))
	{
		agent->m_intermediateGoalPosition = agent->m_currentPath.at(agent->m_currentPath.size() - 1);

		agent->m_forward = agent->m_intermediateGoalPosition - agent->m_position;
		agent->m_forward.NormalizeAndGetLength();

		agent->m_position += (agent->m_forward * (agent->m_movespeed * GetMasterDeltaSeconds()));
	}
	else
	{
		//if we are down to our final destination and we are in the same tile, just snap to the location in that tile
		if (agent->m_currentPath.size() == 1)
		{
			agent->m_position = agent->m_currentPath.at(agent->m_currentPath.size() - 1);
			agent->m_currentPath.erase(agent->m_currentPath.end() - 1);
			return true;
		}
		else
		{
			agent->m_currentPath.erase(agent->m_currentPath.end() - 1);

			if (agent->m_currentPath.size() != 0)
				agent->m_intermediateGoalPosition = agent-> m_currentPath.at(agent->m_currentPath.size() - 1);

			agent->m_forward = agent->m_intermediateGoalPosition - agent->m_position;
			agent->m_forward.NormalizeAndGetLength();

			agent->m_position += (agent->m_forward * (agent->m_movespeed * GetMasterDeltaSeconds()));
		}
	}


	agent->m_transform.SetLocalPosition(agent->m_position);

	return isComplete;
}

//  =========================================================================================
bool ShootAction(Agent* agent, const Vector2& goalDestination, int interactEntityId)
{
	if (!agent->GetIsAtPosition(goalDestination))
	{
		ActionData* data = new ActionData();
		data->m_action = MoveAction;
		data->m_finalGoalDestination = goalDestination;

		agent->AddActionToStack(data);
		return false;
	}

	//if we are at our destination, we are ready to shoot	
	return false;
}

//  =========================================================================================
bool GatherAction(Agent* agent, const Vector2& goalDestination, int interactEntityId)
{
	agent->m_animationSet->SetCurrentAnim("idle");
	// easy outs ----------------------------------------------
	if (!agent->GetIsAtPosition(goalDestination))
	{
		ActionData* data = new ActionData();
		data->m_action = MoveAction;
		data->m_finalGoalDestination = goalDestination;
		data->m_interactEntityId = -1;	//move actions don't have a target entity to interact with

		agent->AddActionToStack(data);
		data = nullptr;
		return false;
	}

	//if we are at our destination, we are ready to gather
	PointOfInterest* targetPoi = agent->m_mapReference->GetPointOfInterestById(interactEntityId);

	//confirm agent is at targetPOI accessLocation
	if (agent->m_mapReference->GetTileCoordinateOfPosition(agent->m_position) != targetPoi->m_accessCoordinate)
	{	
		ActionData* data = new ActionData();
		data->m_action = MoveAction;
		data->m_finalGoalDestination = targetPoi->m_mapReference->GetWorldPositionOfMapCoordinate(targetPoi->m_accessCoordinate);
		data->m_interactEntityId = -1; //move actions don't have a target entity to interact with

		agent->AddActionToStack(data);
		data = nullptr;
		return false;
	}

	//if we are serving another agent or no one is assigned we either need to wait or set this agent to currently serving
	if (targetPoi->m_agentCurrentlyServing != agent)
	{
		//no one is being served, we can begin acquiring resources from the poi
		if (targetPoi->m_agentCurrentlyServing == nullptr)
		{
			targetPoi->m_agentCurrentlyServing = agent;
			targetPoi->m_refillTimer->Reset();
			agent->m_animationSet->SetCurrentAnim("cast");
		}
		//another agent is being served so we need to wait
		else
		{
			//cleanup and return
			targetPoi = nullptr;
			return false;
		}
	}

	// we are ready to be served by poi ----------------------------------------------
	switch (targetPoi->m_type)
	{
	case ARMORY_POI_TYPE:
		if (targetPoi->m_refillTimer->ResetIfElapsed())
		{
			agent->m_arrowCount++;
		}
		if (agent->m_arrowCount == g_maxResourceCarryAmount)
		{
			//agent is served and ready to move on
			targetPoi->m_agentCurrentlyServing = nullptr;

			//cleanup
			targetPoi = nullptr;
			return true;
		}
		break;
	case LUMBERYARD_POI_TYPE:
		if (targetPoi->m_refillTimer->ResetIfElapsed())
		{
			agent->m_lumberCount++;
		}
		if (agent->m_lumberCount == g_maxResourceCarryAmount)
		{
			//agent is served and ready to move on
			targetPoi->m_agentCurrentlyServing = nullptr;

			//cleanup
			targetPoi = nullptr;
			return true;
		}
		break;
	case MED_STATION_POI_TYPE:
		if (targetPoi->m_refillTimer->ResetIfElapsed())
		{
			agent->m_bandageCount++;
		}
		if (agent->m_bandageCount == g_maxResourceCarryAmount)
		{
			//agent is served and ready to move on
			targetPoi->m_agentCurrentlyServing = nullptr;

			//cleanup
			targetPoi = nullptr;
			return true;
		}
		break;
	}		
	
	targetPoi = nullptr;
	return false;
}

