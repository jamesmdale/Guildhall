#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\core\Time.hpp"


TheApp* g_TheApp = nullptr;
double previousTime = 0;

TheApp::TheApp()
{
	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_Game = new Game();
}

TheApp::~TheApp()
{
	delete g_Game;
	g_Game = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
}

void TheApp::RunFrame()
{
	g_theRenderer->BeginFrame();	
	g_theInput->BeginFrame();

	Update();
	Render();

	g_theInput->EndFrame();
	g_theRenderer->EndFrame();	
	
}

void TheApp::Initialize()
{

}

//theapp.beginFrame - begin frame for engine components
void TheApp::Update()
{
	double currentTime = GetCurrentTimeSeconds();
	float timeDelta = (float)(currentTime - previousTime);
	previousTime = currentTime;

	g_Game->Update(timeDelta); 
}

void TheApp::Render()
{
	//set up screen
	g_theRenderer->SetOrtho(g_bottomLeftOrtho, g_topRightOrtho);
	g_theRenderer->ClearColor(g_backgroundColor);
	g_theRenderer->Clear();
	
	g_Game->Render();
}

bool TheApp::IsQuitting()
{	
	bool isQuitting = g_theInput->IsKeyPressed(g_theInput->KEYBOARD_ESCAPE);
	return isQuitting;
}

