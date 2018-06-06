#include "Game\Menu\MenuState.hpp"
#include "Engine\Renderer\Renderer.hpp"

MenuState* g_currentState = nullptr;
MenuState* g_transitionState = nullptr;
float s_secondsInState = 0.0f;
float s_secondsTransistioning = 0.0f;
bool s_isFinishedTransitioningOut = true;
bool s_isFinishedTransitioningIn = true;

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
}

void MenuState::Update(float deltaTime)
{
	UNUSED(deltaTime);
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


float MenuState::UpdateFromInput(float deltaTime)
{
	//input update tasks here
	UNUSED(deltaTime);
}

void MenuState::ResetMenuState()
{
	//after transitioning, reset the state here
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

//static methods

void MenuState::UpdateGlobalMenuState(float timeDelta)
{	
	s_secondsInState += timeDelta;

	if (g_transitionState != nullptr)
	{
		s_secondsTransistioning += timeDelta;

		//make sure the current state and transition state complete their transitions
		if (s_isFinishedTransitioningOut == false)
		{
			g_currentState->TransitionOut(s_secondsTransistioning);
		}
		else if (s_isFinishedTransitioningIn == false)
		{
			g_transitionState->TransitionIn(s_secondsTransistioning);
		}
		else
		{
			//reset transition state
			g_currentState = g_transitionState;
		}
	}
	
}

void MenuState::TransitionMenuStates(MenuState* toState)
{
	g_transitionState = toState;
	s_isFinishedTransitioningOut = false;
	s_isFinishedTransitioningIn = false;
}

void MenuState::TransitionMenuStatesImmediate( MenuState* toState)
{
	FinishTransition();
	g_currentState = toState;
}

MenuState* MenuState::GetCurrentMenuState()
{
	return g_currentState;
}

MenuState* MenuState::GetTransitionMenuState()
{
	return g_transitionState;
}


void FinishTransition()
{
	g_currentState = g_transitionState;
	s_isFinishedTransitioningIn = true;
	s_isFinishedTransitioningOut = true;
	s_secondsInState = 0.0f;
	s_secondsTransistioning = 0.0f;
}
