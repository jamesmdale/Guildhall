#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Time\Time.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\File\ObjectFileLoader.hpp"

TheApp* g_theApp = nullptr;

TheApp::TheApp()
{ 
	g_theGame = new Game();
}

TheApp::~TheApp()
{
	delete g_theGame;
	g_theGame = nullptr;

	TODO("Cleanup (delete and null out) input system and renderer");
}

void TheApp::RunFrame()
{
	Renderer::GetInstance()->BeginFrame();	
	InputSystem::GetInstance()->BeginFrame();
	MasterClockBeginFrame();

	Update();
	PreRender();
	Render();
	PostRender();

	DebugRender::GetInstance()->EndFrame();
	InputSystem::GetInstance()->EndFrame();
	Renderer::GetInstance()->EndFrame();

	Sleep(1);
	TODO("Need to add sleep function to release CPU cycles and reduce system demand");

}

void TheApp::Initialize()
{
	//register app commands
	CommandRegister("quit", CommandRegistration(Quit, ": Use to quit the program", "Quitting..."));

	//start the masterclock
	Clock* masterClock = GetMasterClock();
	masterClock->ClockSystemStartup();
	masterClock = nullptr;	

	//init mouse input settings
	InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(true);
	InputSystem::GetInstance()->GetMouse()->MouseShowCursor(false);
	InputSystem::GetInstance()->GetMouse()->SetMouseMode(MOUSE_RELATIVE_MODE);	

	std::vector<Vector3> vertices;
	std::vector<Vector2> uvs;
	std::vector<float> indicies;
	std::vector<Vector3> normals;

	//g_theGame->Initialize();
}

void TheApp::Update()
{
	float deltaSeconds = GetMasterDeltaTime();

	deltaSeconds = UpdateInput(deltaSeconds);

	float gamedelta = (float)g_gameClock->GetDeltaSeconds();

	//g_theGame->Update(gamedelta); 

	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Update(deltaSeconds);
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Update(deltaSeconds);
	}
}

void TheApp::PreRender()
{
	//g_theGame->PreRender();
}

void TheApp::Render()
{
	//set up screen

	Window* theWindow = Window::GetInstance();
	//Renderer::GetInstance()->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);

	theWindow = nullptr;

	//Renderer::GetInstance()->Clear();

	//g_theGame->Render();

	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Render();
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Render();
	}	
}

void TheApp::PostRender()
{
	//g_theGame->PostRender();
}


float TheApp::UpdateInput(float deltaSeconds)
{
	UNUSED(deltaSeconds); //remove if deltaSeconds needed

	if(InputSystem::GetInstance()->WasKeyJustPressed((InputSystem::GetInstance()->KEYBOARD_TILDE)))
	{
		if(!DevConsole::GetInstance()->IsOpen())
		{
			DevConsole::GetInstance()->Open();
		}
		else
		{
			DevConsole::GetInstance()->Close();
		}		
	}

	if(InputSystem::GetInstance()->WasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
	{
		g_isQuitting = true;
	}

	return deltaSeconds;
}


void Quit(Command &cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}
