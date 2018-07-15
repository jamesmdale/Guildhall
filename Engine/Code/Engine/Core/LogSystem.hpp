#pragma once
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Utility\ThreadSafeQueue.hpp"
#include "Engine\File\File.hpp"
#include "Engine\Core\Command.hpp"
#include <thread>
#include <string>
#include <fstream>

enum eLogTagMode
{
	TAG_MODE_BLACKLIST,
	TAG_MODE_WHITELIST,
	NONE_TAG_MODE
};

class DevConsole;
struct LogEntry
{
	std::string m_tag;
	std::string m_text;
};

typedef void(*LogCallback)(const LogEntry& log, void* arguments);

struct LogHook
{
	LogCallback m_hookCallback;
	void* m_userArguments;
};

class LogSystem
{
public:
	LogSystem();
	~LogSystem();

public:
	static LogSystem* GetInstance();
	static LogSystem* CreateInstance();

	void Startup();
	void Shutdown();

	static void LogThreadWorker(void*);

public:
	// print tags
	void LogTaggedPrint(const char* tag, const char* format, va_list args);
	void LogTaggedPrintf(const char* tag, const char* format, ...);

	//log print
	void LogPrintf(const char* format, ...);
	void LogWarningf(const char* format, ...);
	void LogErrorf(const char* format, ...);

	//log hide/show
	eLogTagMode GetLogMode();
	void LogBlacklistTags();
	void LogWhitelistTags();
	void LogShowTag(const char* tag);
	void LogHideTag(const char* tag);

	//hook functions
	void HookIntoLog(LogCallback callback, void* userArguments);
	void UnhookIntoLog(LogCallback callback, void * userArguments);
	//void UnhookIntoLog(LogCallback callback, void* userArguments);

	//utility functions
	bool IsRunning(){return m_isRunning;}
	void StopLogger(){m_isRunning = false;}

	//flushes while log system is running
	void FlushLoop();
	
	//message queue flush
	void FlushMessages();

public:
	bool m_isRunning = false;
	ThreadSafeQueue<LogEntry*> m_logQueue;
	std::thread* m_logThread = nullptr;
};

void WriteToFile(const LogEntry& log, void* filePointer);

bool IsEntryFilteredOut(const LogEntry & log);

