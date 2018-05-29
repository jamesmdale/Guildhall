#include "Game\MovementAction.hpp"
#include "Engine\Math\MathUtils.hpp"


MovementAction::MovementAction(std::vector<int> orderedPath, Actor* actor, Map* currentMap, Encounter* currentEncounter)
{
	//set data
	m_orderedTileIndicesForShortestPath = orderedPath;
	m_actor = actor;
	m_map = currentMap;
	m_currentEncounter = currentEncounter;
}

void MovementAction::Initialize()
{
	//set states
	m_currentEncounter->m_isProcessingAction = true;
	m_currentTargetIndex = (int)m_orderedTileIndicesForShortestPath.size() - 1;
	m_actor->m_animationSet->SetCurrentAnim("walk");

	m_map->ResetActionableTiles();
}


MovementAction::~MovementAction()
{
}

void MovementAction::Update(float deltaSeconds)
{
	IntVector2 currentTargetPosition = m_map->GetBlockPositionByIndexXZ(m_orderedTileIndicesForShortestPath[m_currentTargetIndex]);

	if(m_totalFractionOfMovementToCurrentIndex == 1.f)
	{	
		if(currentTargetPosition == m_map->GetBlockPositionByIndexXZ(m_orderedTileIndicesForShortestPath[0]))
		{			
			m_actor->SetPositionXZ(currentTargetPosition);
			CleanupState();
			return;
		}		

		m_currentTargetIndex--;
		currentTargetPosition = m_map->GetBlockPositionByIndexXZ(m_orderedTileIndicesForShortestPath[m_currentTargetIndex]);
		 //get the next target tile to move toward.
		m_actor->TurnTowardTargetLocation(currentTargetPosition);
		m_actor->m_positionXYZ.y = m_map->GetHeightForPositionXZ(currentTargetPosition) - 1.f;
		m_totalFractionOfMovementToCurrentIndex = 0.0f;
		m_startPositionForTileMovement = m_actor->m_positionXYZ;		
	}
	else
	{
		//hardcoding move speed for now.
		Vector2 positionMovedThisFrame = Vector2(m_actor->m_currentForwardInMapDirection) * (deltaSeconds * 2.f);

		if(m_actor->m_currentForwardInMapDirection.x > 0.f || m_actor->m_currentForwardInMapDirection.x < 0.f)
		{
			m_totalFractionOfMovementToCurrentIndex += GetAbsoluteValue(positionMovedThisFrame.x);
		}
		else
		{
			m_totalFractionOfMovementToCurrentIndex += GetAbsoluteValue(positionMovedThisFrame.y);
		}

		m_totalFractionOfMovementToCurrentIndex = ClampFloatZeroToOne(m_totalFractionOfMovementToCurrentIndex);

		m_actor->m_positionXYZ = Interpolate(m_startPositionForTileMovement, Vector3(currentTargetPosition.x, m_actor->m_positionXYZ.y, currentTargetPosition.y), m_totalFractionOfMovementToCurrentIndex);
	}	
}

void MovementAction::CleanupState() //clear any map/encounter stuff you were playing with while this is happening.  Change actor animation back
{
	//reset menu
	m_currentEncounter->m_currentMenu->DisableOption(m_currentEncounter->m_currentMenu->GetOptionIndexByName("Move"));
	m_currentEncounter->m_currentMenu->m_currentSelectedOption = -1;
	m_currentEncounter->m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);	

	//reset actor
	m_actor->m_animationSet->SetCurrentAnim("idle");
	m_map->m_cursorPosition = m_actor->GetPositionInXZ();
	m_map->m_currentSelectedActorPosition = m_actor->GetPositionInXZ();	

	//reset processing action
	m_currentEncounter->m_isProcessingAction = false;
}