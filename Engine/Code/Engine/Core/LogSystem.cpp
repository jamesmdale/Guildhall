#include "Engine\Core\LogSystem.hpp"
#include <fstream>

static LogSystem* g_theLogSystem = nullptr;
std::ofstream* g_logFile = nullptr;

//  =============================================================================
LogSystem::LogSystem()
{

}

//  =============================================================================
LogSystem::~LogSystem()
{
	delete(g_logFile);
	g_logFile = nullptr;

	delete(g_theLogSystem);
	g_theLogSystem = nullptr;
}

//  =============================================================================
LogSystem* LogSystem::GetInstance()
{
	return g_theLogSystem; 
}

//  =============================================================================
LogSystem* LogSystem::CreateInstance()
{
	if (g_theLogSystem == nullptr) 
	{
		g_theLogSystem = new LogSystem(); 
	}
	return g_theLogSystem;
}

//  =============================================================================
void LogSystem::Startup()
{
	m_logThread = new std::thread(LogThreadWorker, nullptr);

	//register file to system
	g_logFile = new std::ofstream("Data/Log/log.dat");
}

//  =============================================================================
void LogSystem::Shutdown()
{
	g_theLogSystem->StopLogger();
	m_logThread->join();
}

//  =============================================================================
void LogSystem::FlushMessages()
{
	
}

//  =============================================================================
void LogSystem::LogThreadWorker(void*)
{
	while (g_theLogSystem->IsRunning())
	{
		//flush the message queue
		g_theLogSystem->FlushMessages();

		//sleep the thread for 10ms
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	//flush one more time before shutting down
	g_theLogSystem->FlushMessages();
}

//  =============================================================================
void LogSystem::LogTaggedPrint(const char * tag, const char * format, va_list args)
{
}

//  =============================================================================
void LogSystem::LogTaggedPrintf(const char * tag, const char * format, ...)
{
}

//  =============================================================================
void LogSystem::LogPrintf(const char * format, ...)
{
}

//  =============================================================================
void LogSystem::LogWarningf(const char * format, ...)
{
}

//  =============================================================================
void LogSystem::LogErrorf(const char * format, ...)
{
}

//  =============================================================================
void LogSystem::LogShowAll()
{
}

//  =============================================================================
void LogSystem::LogHideAll()
{
}

//  =============================================================================
void LogSystem::LogShowTag(const char * tag)
{
}

//  =============================================================================
void LogSystem::LogHideTag(const char * tag)
{
}

void LogSystem::HookIntoLog(LogCallback callback, void * userArguments)
{
	m_hook
}
