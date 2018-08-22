#include "Game\Menu\MenuState.hpp"
#include "Engine\Renderer\Renderer.hpp"

GameState* g_currentState = nullptr;
GameState* g_transitionState = nullptr;

float GameState::s_secondsInState = 0.0f;
float GameState::s_secondsTransitioning = 0.0f;
bool GameState::s_isFinishedTransitioningOut = true;
bool GameState::s_isFinishedTransitioningIn = true;
std::vector<GameState*> GameState::s_menuStates;

GameState::GameState(Camera* camera)
{
	m_camera = camera;
	m_renderScene = new RenderScene();

	m_renderScene->AddCamera(m_camera);
}

GameState::~GameState()
{	
	delete(m_renderScene);
	m_renderScene = nullptr;

	//game will manage deletion of camera
	m_camera = nullptr; 

	for (int menuStateIndex = 0; menuStateIndex < (int)s_menuStates.size(); menuStateIndex++)
	{
		delete(s_menuStates[menuStateIndex]);
	}
	s_menuStates.clear();
}

void GameState::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void GameState::PreRender()
{
	//prerender tasks here
}

void GameState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_camera);

	//always do this first at the beginning of the frame's render
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	//render from forward rendering path
	Game::GetInstance()->m_forwardRenderingPath->Render(m_renderScene);

	theRenderer = nullptr;
}

void GameState::PostRender()
{
	//postrender tasks here
}


float GameState::UpdateFromInput(float deltaSeconds)
{
	return deltaSeconds;
}

void GameState::TransitionIn(float secondsTransitioning)
{
	/*	does whatever it wants in terms of transitioning. when finished,
	set s_isFinishedTransitioningIn to true
	*/
	s_isFinishedTransitioningIn = true;
}

void GameState::TransitionOut(float secondsTransitioning)
{
	/*	does whatever it wants in terms of transitioning. when finished,
		set s_isFinishedTransitioningOut to true
	*/
	s_isFinishedTransitioningOut = true;
}

void GameState::ResetState()
{
	//reset menu options or game states here
}

void GameState::Initialize()
{
	//run initialization tasks here
	m_isInitialized = true;
}

//static methods

void GameState::UpdateGlobalMenuState(float deltaSeconds)
{	
	s_secondsInState += deltaSeconds;

	if (g_transitionState != nullptr)
	{
		s_secondsTransitioning += deltaSeconds;

		//make sure the current state and transition state complete their transitions
		if (s_isFinishedTransitioningOut == false)
		{
			g_currentState->TransitionOut(s_secondsTransitioning);
		}
		else if (s_isFinishedTransitioningIn == false)
		{
			g_transitionState->TransitionIn(s_secondsTransitioning);
		}
		else
		{
			//reset transition state
			g_currentState = g_transitionState;
			FinishTransition();
		}
	}
	
}

void GameState::FinishTransition()
{
	g_currentState = g_transitionState;
	g_transitionState = nullptr;
	s_isFinishedTransitioningIn = true;
	s_isFinishedTransitioningOut = true;
	s_secondsInState = 0.0f;
	s_secondsTransitioning = 0.0f;

	if (!g_currentState->IsInitialized())
	{
		g_currentState->Initialize();
	}
	
}


void GameState::TransitionMenuStates(GameState* toState)
{
	GUARANTEE_OR_DIE(toState != nullptr, "INVALID MENU STATE TRANSITION");

	g_transitionState = toState;
	s_isFinishedTransitioningOut = false;
	s_isFinishedTransitioningIn = false;
}

void GameState::TransitionMenuStatesImmediate( GameState* toState)
{
	GUARANTEE_OR_DIE(toState != nullptr, "INVALID MENU STATE TRANSITION");
	g_transitionState = toState;
	s_isFinishedTransitioningOut = true;
	s_isFinishedTransitioningIn = true;
}

GameState* GameState::GetCurrentMenuState()
{
	return g_currentState;
}

GameState* GameState::GetTransitionMenuState()
{
	return g_transitionState;
}

//For now, we assume they will only ever have one of each possible type in the list
GameState* GameState::GetMenuStateFromGlobalListByType(eGameState menuStateType)
{
	for (int menuStateIndex = 0; menuStateIndex < (int)s_menuStates.size(); menuStateIndex++)
	{
		if (s_menuStates[menuStateIndex]->m_type == menuStateType)
		{
			return s_menuStates[menuStateIndex];
		}
	}

	return nullptr;
}

//For now, we assume they will only ever have one of each possible type in the list
void GameState::AddMenuState(GameState* menuState)
{
	s_menuStates.push_back(menuState);
}

float GameState::GetSecondsInCurrentState()
{
	return s_secondsInState;
}


