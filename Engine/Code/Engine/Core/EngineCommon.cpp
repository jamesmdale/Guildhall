#include "Game\EngineBuildPreferences.hpp"
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

#ifdef REMOTE_COMMAND_ENABLED || NET_SESSION_ENABLED
	Net::Startup();
#endif

#ifdef REMOTE_COMMAND_ENABLED
	RemoteCommandService::CreateInstance();
	RemoteCommandService::GetInstance()->Startup();
#endif

#ifdef NET_SESSION_ENABLED
	NetSession::CreateInstance();
	NetSession::GetInstance()->Startup();
#endif
}

//  =============================================================================
void EngineShutdown()
{
#ifdef REMOTE_COMMAND_ENABLED
	RemoteCommandService::GetInstance()->Shutdown();
#endif

#ifdef NET_SESSION_ENABLED
	NetSession::GetInstance()->Shutdown();
#endif

#ifdef REMOTE_COMMAND_ENABLED || NET_SESSION_ENABLED
	Net::Shutdown();	
#endif

	ProfilerConsole::GetInstance()->Shutdown();

	Profiler::GetInstance()->Shutdown();

	AudioSystem::GetInstance()->Shutdown();

	DebugRender::GetInstance()->Shutdown();

	DevConsole::GetInstance()->Shutdown();

	Renderer::GetInstance()->Shutdown();

	LogSystem::GetInstance()->Shutdown();
}
	
