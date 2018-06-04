#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\core\Time.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\DevConsole.hpp"


TheApp* g_theApp = nullptr;
double previousTime = 0;

TheApp::TheApp()
{
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

	TODO("Need to add sleep function to release CPU cycles and reduce system demand")
	
}

void TheApp::Initialize()
{
	//register app commands
	CommandRegister("quit", CommandRegistration(Quit, ": Use to quit the program", "Quitting..."));
}

//theapp.beginFrame - begin frame for engine components
void TheApp::Update()
{
	double currentTime = GetCurrentTimeSeconds();
	float timeDelta = (float)(currentTime - previousTime);
	previousTime = currentTime;

	timeDelta = UpdateFlagsFromInput(timeDelta);
	g_Game->Update(timeDelta); 

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Update(timeDelta);
	}
}


float TheApp::UpdateFlagsFromInput(float timeDelta)
{
	UNUSED(timeDelta); //remove if timeDelta needed

	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_TILDE)))
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
		if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_0)))
		{
			g_theRenderer->LoadErrorShader();
		}
	}
	
	return timeDelta;
}

void TheApp::Render()
{
	//set up screen

	Window* theWindow = Window::GetInstance();
	g_theRenderer->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	theWindow = nullptr;
	delete(theWindow);

	g_theRenderer->Clear();
	
	g_Game->Render();

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Render();
	}
}

void Quit(Command &cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}
