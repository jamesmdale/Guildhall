#include "Engine\Core\LogSystem.hpp"
#include "Engine\Core\DevConsole.hpp"
#include <ctime>
#include <stdarg.h>
#include <fstream>


static LogSystem* g_theLogSystem = nullptr;
static std::ofstream* g_logFile = nullptr;
static std::ofstream* g_datedLogFile = nullptr;

std::vector<LogHook*> g_registeredLogHooks;
std::vector<std::string> g_logTags;

eLogTagMode g_tagMode = TAG_MODE_BLACKLIST;

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
	m_isRunning = true;
	m_logThread = new std::thread(LogThreadWorker, nullptr);	

	//clear and register file to system
	std::remove("Data/Log/log.dat");

	//std::time_t t = std::time(0);   // get time now
	//std::tm* now = std::localtime_s(&t);

	struct tm newTime;
	std::time_t now = std::time(0);
	localtime_s(&newTime, &now);

	g_logFile = new std::ofstream("Data/Log/log.txt");
	g_datedLogFile = new std::ofstream(Stringf("Data/Log/log_%i_%i_%i_%i_%i_%i.txt", newTime.tm_mday, newTime.tm_mon, newTime.tm_year, newTime.tm_hour, newTime.tm_min, newTime.tm_sec).c_str());

	if(g_logFile->is_open() && g_datedLogFile->is_open())
		HookIntoLog(WriteToFile, g_logFile);

	LogPrintf("%s\n", "******************************************************************************************************************************");
}

//  =============================================================================
void LogSystem::Shutdown()
{
	g_logFile->close();

	g_theLogSystem->StopLogger();
	m_logThread->join();

	//clear registered lsit
	for (int hookIndex = 0; hookIndex < g_registeredLogHooks.size(); ++hookIndex)
	{
		delete(g_registeredLogHooks[hookIndex]);
		g_registeredLogHooks[hookIndex] = nullptr;
	}

	delete(g_theLogSystem);
	g_theLogSystem = nullptr;
}

//  =============================================================================
void LogSystem::FlushLoop()
{
	while (m_isRunning)
	{
		FlushMessages();
	}
}

//  =========================================================================================
void LogSystem::FlushMessages()
{
	LogEntry* entry = nullptr;
	bool hasItem = m_logQueue.dequeue(&entry);

	if (hasItem)
	{
		if (!IsEntryFilteredOut(*entry))
		{
				for (int hookIndex = 0; hookIndex < (int)g_registeredLogHooks.size(); ++hookIndex)
			{
				g_registeredLogHooks[hookIndex]->m_hookCallback(*entry, g_registeredLogHooks[hookIndex]->m_userArguments);
			}
		}		
	}

	if (entry != nullptr)
	{
		delete(entry);
		entry = nullptr;
	}
}

//  =============================================================================
void LogSystem::LogThreadWorker(void*)
{
	while (g_theLogSystem->IsRunning())
	{
		//flush the message queue
		g_theLogSystem->FlushLoop();

		//sleep the thread for 10ms
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	//flush one more time before shutting down
	g_theLogSystem->FlushLoop();
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

eLogTagMode LogSystem::GetLogMode()
{
	return g_tagMode;
}

//  =============================================================================
void LogSystem::LogBlacklistTags()
{
	g_logTags.clear();
	g_tagMode = TAG_MODE_BLACKLIST;
}

//  =============================================================================
void LogSystem::LogWhitelistTags()
{
	g_logTags.clear();
	g_tagMode = TAG_MODE_WHITELIST;
}

//  =============================================================================
void LogSystem::LogShowTag(const char * tag)
{
	if (g_tagMode == TAG_MODE_WHITELIST)
	{
		g_logTags.push_back(tag);
	}	
	else if (g_tagMode == TAG_MODE_BLACKLIST)
	{
		for (int tagIndex = 0; tagIndex < (int)g_logTags.size(); ++tagIndex)
		{
			if (g_logTags[tagIndex] == tag)
			{
				g_logTags.erase(g_logTags.begin() + tagIndex);
				return;
			}
		}
	}		
}

//  =============================================================================
void LogSystem::LogHideTag(const char * tag)
{
	if (g_tagMode == TAG_MODE_BLACKLIST)
	{
		g_logTags.push_back(tag);
	}
	else if (g_tagMode == TAG_MODE_WHITELIST)
	{
		for (int tagIndex = 0; tagIndex < (int)g_logTags.size(); ++tagIndex)
		{
			if (g_logTags[tagIndex] == tag)
			{
				g_logTags.erase(g_logTags.begin() + tagIndex);
				return;
			}
		}
	}
}

//  =============================================================================
void LogSystem::HookIntoLog(LogCallback callback, void* userArguments)
{
	//check if callback is already registered
	for (int hookIndex = 0; hookIndex < (int)g_registeredLogHooks.size(); ++hookIndex)
	{
		if (g_registeredLogHooks[hookIndex]->m_hookCallback == callback)
		{
			return;
		}
	}

	//if hook wasn't found, insert it into the last
	LogHook* hook = new LogHook();
	hook->m_hookCallback = callback;
	hook->m_userArguments = userArguments;

	g_registeredLogHooks.push_back(hook);
}

void LogSystem::UnhookIntoLog(LogCallback callback, void* userArguments)
{
	//check if callback is already registered
	for (int hookIndex = 0; hookIndex < (int)g_registeredLogHooks.size(); ++hookIndex)
	{
		if (g_registeredLogHooks[hookIndex]->m_hookCallback == callback)
		{
			g_registeredLogHooks[hookIndex] = nullptr;
			g_registeredLogHooks.erase(g_registeredLogHooks.begin() + hookIndex);
		}
	}
}


//  =============================================================================
void WriteToFile(const LogEntry& log, void* filePointer)
{
	std::string output = Stringf("%s: %s", log.m_tag.c_str(), log.m_text.c_str());

	bool doesPrint = true;

	*g_logFile << output << "\n";
	*g_datedLogFile << output << "\n";
}

//  =========================================================================================

bool IsEntryFilteredOut(const LogEntry& log)
{
	switch (g_tagMode)
	{
	case TAG_MODE_BLACKLIST:
	{
		for (int tagIndex = 0; tagIndex < (int)g_logTags.size(); ++tagIndex)
		{
			if (log.m_tag == g_logTags[tagIndex])
			{
				return true;
			}
		}

		return false;
		break;
	}

	case TAG_MODE_WHITELIST:
	{
		for (int tagIndex = 0; tagIndex < (int)g_logTags.size(); ++tagIndex)
		{
			if (log.m_tag == g_logTags[tagIndex])
			{
				return false;
			}
		}

		return true;
	}
	}

	return false;
}