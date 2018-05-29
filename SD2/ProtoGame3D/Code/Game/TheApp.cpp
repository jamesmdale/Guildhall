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
	Render();

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

	g_theGame->Initialize();
}

void TheApp::Update()
{
	float deltaSeconds = GetMasterDeltaTime();

	deltaSeconds = ClampFloat(deltaSeconds, 0.0f, 0.05f);

	deltaSeconds = UpdateFlagsFromInput(deltaSeconds);

	float gamedelta = g_gameClock->GetDeltaSeconds();
	g_theGame->Update(gamedelta); 

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Update(deltaSeconds);
	}
}

void TheApp::Render()
{
	//set up screen

	Window* theWindow = Window::GetInstance();
	//Renderer::GetInstance()->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);

	theWindow = nullptr;

	//Renderer::GetInstance()->Clear();

	g_theGame->Render();

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Render();
	}	
}


float TheApp::UpdateFlagsFromInput(float deltaSeconds)
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

	if(!DevConsole::GetInstance()->IsOpen())
	{
		if(InputSystem::GetInstance()->WasKeyJustPressed((InputSystem::GetInstance()->KEYBOARD_0)))
		{
			Renderer::GetInstance()->LoadErrorShader();
		}
	}

	return deltaSeconds;
}


void Quit(Command &cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}
