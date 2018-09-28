#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\LogSystem.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Profiler\ProfilerConsole.hpp"
#include "Engine\Net\Net.hpp"
#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\NetSession.hpp"


Blackboard g_gameConfigBlackboard;

//  =============================================================================
void EngineStartup()
{
	LogSystem::CreateInstance();
	LogSystem::GetInstance()->Startup();

	RenderStartup();

	Renderer::CreateInstance();
	Renderer::GetInstance()->PostStartup();

	DevConsole::CreateInstance();
	DevConsole::GetInstance()->Startup();

	DebugRender::CreateInstance();
	DebugRender::GetInstance()->Startup();

	AudioSystem::CreateInstance();
	AudioSystem::GetInstance()->Startup();

	Profiler::CreateInstance();
	Profiler::GetInstance()->Startup();

	ProfilerConsole::CreateInstance();
	ProfilerConsole::GetInstance()->Startup();

	Net::Startup();

	RemoteCommandService::CreateInstance();
	RemoteCommandService::GetInstance()->Startup();

	NetSession::CreateInstance();
	NetSession::GetInstance()->Startup();
}

//  =============================================================================
void EngineShutdown()
{
	ProfilerConsole::GetInstance()->Shutdown();

	Profiler::GetInstance()->Shutdown();

	AudioSystem::GetInstance()->Shutdown();

	DebugRender::GetInstance()->Shutdown();

	DevConsole::GetInstance()->Shutdown();

	Renderer::GetInstance()->Shutdown();

	LogSystem::GetInstance()->Shutdown();
}
	
