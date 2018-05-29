#include "Game\ShootingAction.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Math\Trajectory.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"


ShootingAction::ShootingAction(Vector3 target, Actor* actor, Map* currentMap, Encounter* currentEncounter)
{
	//set data
	m_target = target;
	m_actor = actor;
	m_map = currentMap;
	m_currentEncounter = currentEncounter;

	m_elapsedTime = 0.0f;
}

void ShootingAction::Initialize()
{
	//set states
	m_currentEncounter->m_isProcessingAction = true;
	m_actor->m_animationSet->SetCurrentAnim("attack");
	m_currentEncounter->m_trajectoryState = PROCESSING_TRAJECTORY;	
	m_map->ResetActionableTiles();

	//update actor
	m_actor->TurnTowardTargetLocation(IntVector2(m_target.x, m_target.z));
	m_projectilePositionXYZ = m_actor->m_positionXYZ;
	
	m_distanceToTarget = GetDistance(Vector2(m_actor->GetPositionInXZ()), Vector2(m_target.x, m_target.z));
	m_heightDifference = (m_target.y - 1.f) - m_actor->m_positionXYZ.y;

	m_launchSpeed = Trajectory::GetMinimumLaunchSpeed(m_gravity, 5.f + 0.5); //get min launch speed for max distance
	Vector2 launchAngles;

	bool doesHaveLaunchAngle = Trajectory::GetLaunchAngles(&launchAngles, m_gravity, m_launchSpeed, m_distanceToTarget, m_heightDifference);
	GUARANTEE_RECOVERABLE(doesHaveLaunchAngle, "Trajectory angle not computed.  Bad data.");
	
	m_launchAngle = launchAngles.y;
}


void ShootingAction::Update(float deltaSeconds)
{
	m_elapsedTime += deltaSeconds;
	bool isPointAtDestination = GetIsPointInsideCube(m_projectilePositionXYZ, m_target, Vector3::ONE);
	if(isPointAtDestination)
	{
		CleanupState();
		return;		
	}

	else
	{
		Vector2 coordinateInTargetAndPositionSpace = Trajectory::Evaluate(m_gravity, m_launchSpeed, m_launchAngle, m_elapsedTime);
		Vector3 displacementVector = Vector3(m_target.x, 0.f, m_target.z) - Vector3(m_actor->m_positionXYZ.x, 0.f, m_actor->m_positionXYZ.z);
		displacementVector.Normalize();		

		m_projectilePositionXYZ = m_actor->m_positionXYZ + (displacementVector * coordinateInTargetAndPositionSpace.x) + (Vector3::UP * coordinateInTargetAndPositionSpace.y);
	}	
}

void ShootingAction::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetCamera(g_theGame->m_gameCamera);

	theRenderer->SetTexture(*theRenderer->m_defaultTexture);
	theRenderer->EnableDepth(LESS, true);
	theRenderer->DrawCube(m_projectilePositionXYZ, Vector3(.25f, .25f, .25f), Rgba::RED);

	theRenderer = nullptr;
}

void ShootingAction::CleanupState() //clear any map/encounter stuff you were playing with while this is happening.  Change actor animation back
{
	//reset menu
	m_currentEncounter->m_currentMenu->DisableOption(m_currentEncounter->m_currentMenu->GetOptionIndexByName("Shoot"));
	m_currentEncounter->m_currentMenu->m_currentSelectedOption = -1;
	m_currentEncounter->m_currentMenu->FindAndSetHoverToFirstEnabledOption(-1);	

	//reset actor
	m_actor->m_animationSet->SetCurrentAnim("idle");
	m_map->m_currentSelectedActorPosition = m_actor->GetPositionInXZ();

	//reset processing action
	m_currentEncounter->m_trajectoryState = FINISHED_TRAJECTORY;
	m_currentEncounter->m_isProcessingAction = false;
}