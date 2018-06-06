#include "Game\Menu\MenuState.hpp"
#include "Engine\Renderer\Renderer.hpp"

MenuState* g_currentState = nullptr;
MenuState* g_transitionState = nullptr;

float MenuState::s_secondsInState = 0.0f;
float MenuState::s_secondsTransitioning = 0.0f;
bool MenuState::s_isFinishedTransitioningOut = true;
bool MenuState::s_isFinishedTransitioningIn = true;
std::vector<MenuState*> MenuState::s_menuStates;

MenuState::MenuState(Camera* camera)
{
	m_camera = camera;
	m_renderScene = new RenderScene();

	m_renderScene->AddCamera(m_camera);
}

MenuState::~MenuState()
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

void MenuState::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void MenuState::PreRender()
{
	//prerender tasks here
}

void MenuState::Render()
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

void MenuState::PostRender()
{
	//postrender tasks here
}


float MenuState::UpdateFromInput(float deltaSeconds)
{
	return deltaSeconds;
}

void MenuState::TransitionIn(float secondsTransitioning)
{
	/*	does whatever it wants in terms of transitioning. when finished,
	set s_isFinishedTransitioningIn to true
	*/
	s_isFinishedTransitioningIn = true;
}

void MenuState::TransitionOut(float secondsTransitioning)
{
	/*	does whatever it wants in terms of transitioning. when finished,
		set s_isFinishedTransitioningOut to true
	*/
	s_isFinishedTransitioningOut = true;
}

void MenuState::ResetState()
{
	//reset menu options or game states here
}

//static methods

void MenuState::UpdateGlobalMenuState(float deltaSeconds)
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

void MenuState::FinishTransition()
{
	g_currentState = g_transitionState;
	g_transitionState = nullptr;
	s_isFinishedTransitioningIn = true;
	s_isFinishedTransitioningOut = true;
	s_secondsInState = 0.0f;
	s_secondsTransitioning = 0.0f;
}


void MenuState::TransitionMenuStates(MenuState* toState)
{
	GUARANTEE_OR_DIE(toState != nullptr, "INVALID MENU STATE TRANSITION");

	g_transitionState = toState;
	s_isFinishedTransitioningOut = false;
	s_isFinishedTransitioningIn = false;
}

void MenuState::TransitionMenuStatesImmediate( MenuState* toState)
{
	GUARANTEE_OR_DIE(toState != nullptr, "INVALID MENU STATE TRANSITION");
	g_transitionState = toState;
	s_isFinishedTransitioningOut = true;
	s_isFinishedTransitioningIn = true;
}

MenuState* MenuState::GetCurrentMenuState()
{
	return g_currentState;
}

MenuState* MenuState::GetTransitionMenuState()
{
	return g_transitionState;
}

//For now, we assume they will only ever have one of each possible type in the list
MenuState* MenuState::GetMenuStateFromGlobalListByType(eMenuState menuStateType)
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
void MenuState::AddMenuState(MenuState* menuState)
{
	s_menuStates.push_back(menuState);
}

float MenuState::GetSecondsInCurrentState()
{
	return s_secondsInState;
}


