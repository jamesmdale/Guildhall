#include "Game\Menu\MenuState.hpp"
#include "Engine\Renderer\Renderer.hpp"

MenuState::MenuState(ForwardRenderingPath* renderingPath, Camera* camera)
{
	m_renderingPath = renderingPath;
	m_camera = camera;
	m_renderScene = new RenderScene();

	m_renderScene->AddCamera(m_camera);
}

MenuState::~MenuState()
{	
	delete(m_renderScene);
	m_renderScene = nullptr;

	//game will manage deletion of camera and itself
	m_camera = nullptr; 
	m_renderingPath = nullptr;	
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
	m_renderingPath->Render(m_renderScene);

	theRenderer = nullptr;
}

void MenuState::PostRender()
{
	//postrender tasks here
}


float MenuState::UpdateFromInput(float deltaTime)
{
	//input update tasks here
	return deltaTime;
}

void MenuState::ResetMenuState()
{
	//after transitioning, reset the state here
}

//static methods
void MenuState::UpdateGlobalMenuState(float timeDelta)
{

}

void MenuState::TransitionMenuStates(const MenuState& toState)
{

}
