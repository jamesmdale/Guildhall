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
#include "Game\Menu\MenuState.hpp"
#include "Engine\Audio\AudioSystem.hpp"

TheApp* g_theApp = nullptr;

TheApp::TheApp()
{ 
	Game::CreateInstance();
}

TheApp::~TheApp()
{
	TODO("DELETE SYSTEMS AND NULL OUT");
}

void TheApp::RunFrame()
{
	Renderer::GetInstance()->BeginFrame();	
	InputSystem::GetInstance()->BeginFrame();
	AudioSystem::GetInstance()->BeginFrame();
	MasterClockBeginFrame();

	Update();
	PreRender();
	Render();
	PostRender();

	AudioSystem::GetInstance()->EndFrame();
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

	Game::CreateInstance();
	Game::GetInstance()->Initialize();
}

void TheApp::Update()
{
	float deltaSeconds = GetMasterDeltaSeconds();

	//update global menu data (handles transitions and timers)
	GameState::UpdateGlobalMenuState(deltaSeconds);

	deltaSeconds = UpdateInput(deltaSeconds);

	Game::GetInstance()->Update();

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
	Game::GetInstance()->PreRender();
}

void TheApp::Render()
{
	//set up screen
	Game::GetInstance()->Render();

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
	Game::GetInstance()->PostRender();
}


float TheApp::UpdateInput(float deltaSeconds)
{
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

	if (!DevConsole::GetInstance()->IsOpen())
	{
		deltaSeconds = Game::GetInstance()->UpdateInput(deltaSeconds);
	}

	return deltaSeconds;
}

// command callbacks =========================================================================================

void Quit(Command &cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}
