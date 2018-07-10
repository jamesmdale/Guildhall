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
#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Profiler\ProfilerConsole.hpp"
#include "Engine\File\File.hpp"
#include "Engine\Async\Threading.hpp"
#include "Engine\Time\Stopwatch.hpp"

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
	//start profiler for frame
	Profiler::GetInstance()->MarkFrame();

	//begin frame for engine systems
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
	CommandRegister("non_threaded_test", CommandRegistration(NoThreadTest, ": use to test if the thread works", "Processing.."));
	CommandRegister("threaded_test", CommandRegistration(ThreadTest, ": use to test if the thread works", "Processing.."));

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
	PROFILER_PUSH();

	float deltaSeconds = GetMasterDeltaSeconds();

	//update global menu data (handles transitions and timers)
	GameState::UpdateGlobalMenuState(deltaSeconds);

	deltaSeconds = UpdateInput(deltaSeconds);

	Game::GetInstance()->Update();

	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Update(deltaSeconds);
	}

	if (ProfilerConsole::GetInstance()->IsOpen())
	{
		ProfilerConsole::GetInstance()->UpdateFromInput();
		ProfilerConsole::GetInstance()->Update();
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Update(deltaSeconds);
	}

}

void TheApp::PreRender()
{
	//PROFILER_PUSH();

	Game::GetInstance()->PreRender();

	if (ProfilerConsole::GetInstance()->IsOpen())
	{
		ProfilerConsole::GetInstance()->PreRender();
	}
}

void TheApp::Render()
{
	PROFILER_PUSH();
	//set up screen
	Game::GetInstance()->Render();

	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Render();
	}

	if (ProfilerConsole::GetInstance()->IsOpen())
	{
		ProfilerConsole::GetInstance()->Render();
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
		if (InputSystem::GetInstance()->WasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_M) && !ProfilerConsole::GetInstance()->IsOpen())
		{
			AudioSystem::GetInstance()->ToggleMasterMute();
		}				

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

void NoThreadTest(Command &cmd)
{
	ThreadTestWork(nullptr);
}

void ThreadTest(Command &cmd)
{
	ThreadCreateAndDetach(ThreadTestWork, nullptr);
}

void ThreadTestWork(void* arguments)
{
	for (int writeIndex = 0; writeIndex < 12000; ++writeIndex)
	{
		int randomInt = GetRandomIntInRange(1, 10);
		WriteToFile("Data/garbage.dat", Stringf("%i", randomInt));
	}	

	DevConsolePrintf(Stringf("Finished thread work in").c_str());
}
