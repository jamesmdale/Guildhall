#include "Engine\Core\LogSystem.hpp"
#include <fstream>
#include <stdarg.h>

static LogSystem* g_theLogSystem = nullptr;
std::ofstream* g_logFile = nullptr;

std::vector<LogHook*> s_registeredLogHooks;

//  =============================================================================
LogSystem::LogSystem()
{

}

//  =============================================================================
LogSystem::~LogSystem()
{
	delete(g_logFile);
	g_logFile = nullptr;

	delete(m_logThread);
	m_logThread = nullptr;
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

	//clear and register file to system
	std::remove("Data/Log/log.dat");
	g_logFile = new std::ofstream("Data/Log/log.dat");

	if(g_logFile->is_open())
		HookIntoLog(WriteToFile, g_logFile);

	LogPrintf("%s\n", "******************************************************************************************************************************");
}

//  =============================================================================
void LogSystem::Shutdown()
{
	g_theLogSystem->StopLogger();
	m_logThread->join();

	//clear registered lsit
	for (int hookIndex = 0; hookIndex < s_registeredLogHooks.size(); ++hookIndex)
	{
		delete(s_registeredLogHooks[hookIndex]);
		s_registeredLogHooks[hookIndex] = nullptr;
	}

	delete(g_theLogSystem);
	g_theLogSystem = nullptr;
}

//  =============================================================================
void LogSystem::FlushMessages()
{
	bool isFlushing = true;

	while (isFlushing)
	{
		LogEntry* entry = nullptr;
		isFlushing = m_logQueue.dequeue(&entry);
	}
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
void LogSystem::LogTaggedPrint(const char* tag, const char* format, va_list args)
{
	LogEntry* entry = new LogEntry();
	entry->m_tag = tag;
	entry->m_text = Stringf(format, args);

	LogSystem::GetInstance()->m_logQueue.enqueue(entry);
}

//  =============================================================================
void LogSystem::LogTaggedPrintf(const char* tag, const char* format, ...)
{
	LogEntry* entry = new LogEntry();
	entry->m_tag = tag;

	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	entry->m_text = textLiteral;

	LogSystem::GetInstance()->m_logQueue.enqueue(entry);
}

//  =============================================================================
void LogSystem::LogPrintf(const char* format, ...)
{
	LogEntry* entry = new LogEntry();
	entry->m_tag = "log";

	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	entry->m_text = textLiteral;

	LogSystem::GetInstance()->m_logQueue.enqueue(entry);
}

//  =============================================================================
void LogSystem::LogWarningf(const char* format, ...)
{
	LogEntry* entry = new LogEntry();
	entry->m_tag = "WARNING";

	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	entry->m_text = textLiteral;

	LogSystem::GetInstance()->m_logQueue.enqueue(entry);
}

//  =============================================================================
void LogSystem::LogErrorf(const char* format, ...)
{
	LogEntry* entry = new LogEntry();
	entry->m_tag = "ERROR";

	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	entry->m_text = textLiteral;

	LogSystem::GetInstance()->m_logQueue.enqueue(entry);
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

//  =============================================================================
void LogSystem::HookIntoLog(LogCallback callback, void* userArguments)
{
	//check if callback is already registered
	for (int hookIndex = 0; hookIndex < (int)s_registeredLogHooks.size(); ++hookIndex)
	{
		if (s_registeredLogHooks[hookIndex]->m_hookCallback == callback)
		{
			return;
		}
	}

	//if hook wasn't found, insert it into the last
	LogHook* hook = new LogHook();
	hook->m_hookCallback = callback;
	hook->m_userArguments = userArguments;

	s_registeredLogHooks.push_back(hook);
}


//  =============================================================================
void WriteToFile(const LogHook& log, void* filePointer)
{

}