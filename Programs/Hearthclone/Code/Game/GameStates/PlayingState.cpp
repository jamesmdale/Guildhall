#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"

PlayingState::~PlayingState()
{
	delete(m_gameBoard);
	m_gameBoard = nullptr;

	//delete scene last
	delete(m_renderScene2D);
	m_renderScene2D = nullptr;	
}

void PlayingState::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	Board* gameBoard = new Board("board");
	gameBoard->m_renderScene = m_renderScene2D;
	gameBoard->Initialize();

	theRenderer = nullptr;	
}

void PlayingState::Update(float deltaSeconds)
{ 
	
}

void PlayingState::PreRender()
{
	
}

void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	/*theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK)*/;

	Game::GetInstance()->m_forwardRenderingPath2D->Render(m_renderScene2D);

	theRenderer = nullptr;
}

void PlayingState::PostRender()
{
	//post render steps here.
}


float PlayingState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();

	//cleanup
	theInput = nullptr;

	return deltaSeconds; //new deltaSeconds
}

