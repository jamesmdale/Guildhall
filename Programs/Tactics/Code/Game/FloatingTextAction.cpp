#include "Game\FloatingTextAction.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"


FloatingTextAction::FloatingTextAction(int index, Actor* targetActor, Encounter* encounter, std::string text, Rgba color)
{
	//setters
	m_indexInPendingActionList = index;
	m_targetActor = targetActor;
	m_encounter = encounter;
	m_text = text;
	m_color = color;

	m_currentTextPosition = Vector3(m_targetActor->m_positionXYZ.x, m_targetActor->m_positionXYZ.y + 1.f, m_targetActor->m_positionXYZ.z);
	m_targetActor->m_animationSet->SetCurrentAnim("hit");	
}

FloatingTextAction::~FloatingTextAction()
{
}

void FloatingTextAction::Initialize()
{
	m_transform.SetPosition(m_currentTextPosition);
	m_transform.SetScale(Vector3(.5f, .5f, 0.f));
	m_transform.SetRotation(Vector3(0.f, 90.f - g_theGame->m_gameCamera->m_rotation, 0.f));
}

void FloatingTextAction::Update(float deltaSeconds)
{
	if(m_elapsedTime == g_floatingTextDuration)
	{
		CleanupState();
		return;
	}
	m_elapsedTime += deltaSeconds;
	m_elapsedTime = ClampFloat(m_elapsedTime, 0.f, g_floatingTextDuration);

	m_currentTextPosition += Vector3::UP * deltaSeconds;

	m_transform.SetPosition(m_currentTextPosition);
	m_transform.SetScale(Vector3(0.5f, 0.5f, 0.5f));
	m_transform.SetRotation(Vector3(0.f, 270.f - g_theGame->m_gameCamera->m_rotation, 0.f));
		
	float timeRemaining = g_floatingTextDuration - m_elapsedTime;
	float fadePercentage = RangeMapFloat(timeRemaining, 0.f, 5.f, 0.f, 1.f);
	float currentTransparency = RangeMapFloat(fadePercentage, 0.f, 1.f, 0.f, 255.f);

	m_color.a = currentTransparency;
}

void FloatingTextAction::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(g_theGame->m_gameCamera);

	theRenderer->SetTexture(*theRenderer->m_defaultTexture);
	theRenderer->EnableDepth(ALWAYS, true);

	theRenderer->DrawOrientedText2DCentered(m_transform.GetTransformMatrix(), m_text, 1.f, m_color, .5f, theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
}

void FloatingTextAction::CleanupState()
{
	if(m_targetActor != nullptr)
	{
		m_targetActor->m_animationSet->SetCurrentAnim("idle");
	}	
	
	for(int actionIndex = 0.f; actionIndex < (int)m_encounter->m_pendingFloatingActions.size(); actionIndex++)
	{
		if(m_encounter->m_pendingFloatingActions[actionIndex]->m_indexInPendingActionList == m_indexInPendingActionList)
		{
			m_encounter->m_pendingFloatingActions.erase(m_encounter->m_pendingFloatingActions.begin() + actionIndex);
		}
	}
}
