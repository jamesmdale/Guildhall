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
#include "Engine\Core\LogSystem.hpp"
#include <thread>
#include <fstream>

TheApp* g_theApp = nullptr;

//  =============================================================================
TheApp::TheApp()
{ 
	Game::CreateInstance();
}

//  =============================================================================
TheApp::~TheApp()
{
	TODO("DELETE SYSTEMS AND NULL OUT");
}

//  =============================================================================
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

//  =============================================================================
void TheApp::Initialize()
{
	//register app commands
	CommandRegister("quit", CommandRegistration(Quit, ": Use to quit the program", "Quitting..."));
	CommandRegister("non_threaded_test", CommandRegistration(NoThreadTest, ": use to test if the thread works", "Processing.."));
	CommandRegister("threaded_test", CommandRegistration(ThreadTest, ": use to test if the thread works", "Processing.."));
	CommandRegister("log_test", CommandRegistration(LogThreadTest, ": use to test if the logging through devconsole works", "Processing.."));
	CommandRegister("log_tag_test", CommandRegistration(LogTagThreadTest, ": use to test multiple tags to test filtering..."));

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

//  =============================================================================
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

//  =============================================================================
void TheApp::PreRender()
{
	//PROFILER_PUSH();

	Game::GetInstance()->PreRender();

	if (ProfilerConsole::GetInstance()->IsOpen())
	{
		ProfilerConsole::GetInstance()->PreRender();
	}
}

//  =============================================================================
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

//  =============================================================================
void TheApp::PostRender()
{
	Game::GetInstance()->PostRender();
}

//  =============================================================================
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

//  =============================================================================
// command callbacks =========================================================================================
//  =============================================================================
void Quit(Command &cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}

//  =============================================================================
void NoThreadTest(Command &cmd)
{
	ThreadTestWork(nullptr);
}

//  =============================================================================
void ThreadTest(Command &cmd)
{
	std::thread threadObject(ThreadTestWork, nullptr);
	threadObject.detach();
}

//  =============================================================================
void ThreadTestWork(void* arguments)
{
	for (int writeIndex = 0; writeIndex < 12000; ++writeIndex)
	{
		int randomInt = GetRandomIntInRange(1, 10);
		WriteToFileImmediate("Data/garbage.dat", Stringf("%i", randomInt));
	}

	DevConsolePrintf(Stringf("Finished thread work").c_str());
}

//  =============================================================================
void LogThreadTest(Command& cmd)
{
	std::string sourceFileName = cmd.GetNextString();
	int threadCount = cmd.GetNextInt();

	if (threadCount == NULL || sourceFileName == "")
	{
		DevConsolePrintf(Rgba::RED, "Log thread not initiated! Invalid thread count. Source file must be in Data/Log/! (default name should be 'Holmes.text')");
		DevConsolePrintf(Rgba::RED, "Follow format: 'log_test FILEPATH THREADCOUNT'");
	}
	else
	{
		LogTest(Stringf("Data/Log/%s", sourceFileName.c_str()).c_str(), threadCount);
		DevConsolePrintf("Log thread test success!");
	}
}

//  =============================================================================
void LogTagThreadTest(Command& cmd)
{
	std::thread threadObject(LogTagTestWork, nullptr);
	threadObject.detach();
}

//  =========================================================================================
struct LogTestInfo
{
	const char* filePath = "";
	int threadId = 0;
};

//  =============================================================================
void LogTest(const char* sourceFile, int threadCount)
{
	for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex)
	{
		LogTestInfo info;
		info.filePath = sourceFile;
		info.threadId = threadIndex + 1;

		std::thread object(LogTestWork, (void*)&info);
		object.detach();
	}
}

//  =============================================================================
void LogTestWork(void* arguments)
{
	LogTestInfo* info = (LogTestInfo*)arguments;

	std::string filePath = info->filePath;
	int threadId = info->threadId;

	std::ifstream streamObj(filePath);

	LogSystem* theLogSystem = LogSystem::GetInstance();

	std::string currentLine;

	int lineNumber = 1;
	if (streamObj.is_open())
	{
		while (streamObj.good())
		{
			getline (streamObj, currentLine);

			theLogSystem->LogPrintf("%i:%i %s", threadId, lineNumber, currentLine.c_str());
			lineNumber++;
		}
		streamObj.close();
	}

	info = nullptr;
	theLogSystem = nullptr;
}

//  =============================================================================
void LogTagTestWork(void* arguments)
{
	int count = 0;
	while (count < 200)
	{
		LogSystem::GetInstance()->LogTaggedPrintf("rock", "%s", "This beats scissors");
		LogSystem::GetInstance()->LogTaggedPrintf("paper", "%s", "This beats rock");
		LogSystem::GetInstance()->LogTaggedPrintf("scissors", "%s", "This is beats paper");

		count++;
	}
}


