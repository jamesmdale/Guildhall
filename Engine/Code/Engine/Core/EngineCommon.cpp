#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Profiler\ProfilerConsole.hpp"

Blackboard g_gameConfigBlackboard;

void EngineStartup()
{
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
}

void EngineShutdown()
{
	ProfilerConsole::GetInstance()->Shutdown();

	Profiler::GetInstance()->Shutdown();

	AudioSystem::GetInstance()->Shutdown();

	DebugRender::GetInstance()->Shutdown();

	DevConsole::GetInstance()->Shutdown();

	Renderer::GetInstance()->Shutdown();
}
	
