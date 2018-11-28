#pragma once
#include "Engine\Time\Time.hpp"
#include <stdint.h>
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include <queue>
#include "Engine\Math\IntRange.hpp"


#define PROFILE_LOG_SCOPE(tag) ProfileLogScoped __timer_ ##__LINE__ ## (tag)
#define PROFILE_LOG_SCOPE_FUNCTION() ProfileLogScoped __timer_ ##__LINE__ ##__FUNCTION__( __FUNCTION__)

#define PROFILER_PUSH() ProfilerScoped __timer_ ##__LINE__ ##__FUNCTION__( __FUNCTION__)

constexpr int MAX_HISTORY_COUNT = 256;

class ProfilerReport;

struct ProfileMeasurement
{
	~ProfileMeasurement()
	{
		m_parent = nullptr;
		DeleteTree();
	}

	char m_id[64];
	uint64_t m_startTime;
	uint64_t m_endTime;

	ProfileMeasurement* m_parent;
	std::vector<ProfileMeasurement*> m_children;

	void SetParent(ProfileMeasurement* parent){ m_parent = parent; }
	void AddChild(ProfileMeasurement* child){m_children.push_back(child);}

	uint64_t GetElapsedTimeHPC(){return m_endTime - m_startTime;}

	void DeleteTree()
	{
		for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
		{
			delete(m_children[childIndex]);
			m_children[childIndex] = nullptr;
		}
	}

	void PrintTree()
	{
		//calculate elapsed seconds
		float secondsElapsed = (float)PerformanceCounterToSeconds(m_endTime - m_startTime);

		//print formatted string for print
		std::string printString = Stringf("ID: %s %f \n", m_id, secondsElapsed);
		DebuggerPrintf(printString.c_str());
		DevConsolePrintf(printString.c_str());
		
		for (int childIndex = 0; childIndex < (int)m_children.size(); ++childIndex)
		{
			m_children[childIndex]->PrintTree();
		}
	}

	void Finish()
	{
		m_endTime = GetPerformanceCounter();
	}
};

// profiler system =============================================================================
class Profiler
{
public:
	Profiler();
	~Profiler();

	static Profiler* CreateInstance();
	static Profiler* GetInstance();

	void Startup();
	void Shutdown();

	ProfileMeasurement* CreateMeasurement(const char* id);
	void DestroyMeasurementTreeRecurssive();
	void PrintHistory();

	ProfileMeasurement* ProfileGetPreviousFrame(int skipCount = 0);
	bool ProfilerGetPreviousFrames(std::vector<ProfileMeasurement*>& history, int numFrames = MAX_HISTORY_COUNT);

	void MarkFrame();
	void Push(const char* id);
	void Pop();

	void PauseProfiler();
	void ResumeProfiler();

	bool IsPaused();

public:
	ProfileMeasurement* m_stack = nullptr;

	int m_frameIndex = 0;
	ProfileMeasurement* m_measurementHistory[MAX_HISTORY_COUNT]; //we keep N number of these to preserve history
};

void Pause(Command& cmd);
void Resume(Command& cmd);
void LogHistory(Command& cmd);
void Report(Command& cmd);

// profile log scope =============================================================================
struct ProfileLogScoped
{
public:
	ProfileLogScoped(const char* tag)
	{
	#ifdef PROFILER_ENABLED
		m_tag = tag;
		m_hpcStart = GetPerformanceCounter();
	#endif
	}

	~ProfileLogScoped()
	{
#ifdef PROFILER_ENABLED
		uint64_t endHpc = GetPerformanceCounter() - m_hpcStart;
		double hpcInSeconds = PerformanceCounterToSeconds(endHpc);
		DevConsolePrintf(Stringf("Tag Name: %s - %f \n", m_tag, (float)hpcInSeconds).c_str());
		DebuggerPrintf(Stringf("Tag Name: %s - %f \n", m_tag, (float)hpcInSeconds).c_str());
#endif
	}

public:
	uint64_t m_hpcStart;
	const char* m_tag;	
};


// profiler scope =============================================================================
struct ProfilerScoped
{
public:
	ProfilerScoped(const char* id)
	{
		#ifdef PROFILER_ENABLED
			Profiler::GetInstance()->Push(id);
		#endif
		
	}

	~ProfilerScoped()
	{
		#ifdef PROFILER_ENABLED
			Profiler::GetInstance()->Pop();
		#endif
	}
};
