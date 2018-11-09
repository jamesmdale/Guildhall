#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Profiler\ProfilerConsole.hpp"
#include "Engine\Profiler\ProfilerReport.hpp"
#include "Game\EngineBuildPreferences.hpp"

static Profiler* g_theProfiler = nullptr;
bool g_isProfilerPaused = false;
bool g_isProfilerPausing = false;
bool g_isProfilerResuming = false;

//  =========================================================================================
Profiler::Profiler()
{
}

//  =========================================================================================
Profiler::~Profiler()
{
}

//  =========================================================================================
Profiler* Profiler::CreateInstance()
{
	if (g_theProfiler == nullptr)
	{
		g_theProfiler = new Profiler();
	}

	return g_theProfiler;
}

//  =========================================================================================
Profiler* Profiler::GetInstance()
{
	return g_theProfiler;
}

//  =========================================================================================
void Profiler::Startup()
{
#ifdef PROFILER_ENABLED
	RegisterCommand("profiler_pause", CommandRegistration(Pause, ": Pauses profiler capturing", "Profiler Paused!"));
	RegisterCommand("profiler_resume", CommandRegistration(Resume, ": Resume profiler capturing", "Profiler resumed!"));
	RegisterCommand("profiler_history", CommandRegistration(LogHistory, ": Print all history items in the profile history capture", "Profiler printing history.."));
	RegisterCommand("profiler_report", CommandRegistration(Report, ": Capture profiler report for last frame", "Profile printing report..."));
#endif
}

//  =========================================================================================
void Profiler::Shutdown()
{
	delete(g_theProfiler);
	g_theProfiler = nullptr;
}

//  =========================================================================================
// utility methods =========================================================================================
//  =========================================================================================
#ifdef PROFILER_ENABLED
ProfileMeasurement* Profiler::CreateMeasurement(const char* id)
{
	ProfileMeasurement* measure = new ProfileMeasurement();

	strncpy_s(measure->m_id, id, 64);
	measure->m_startTime = GetPerformanceCounter();

	return measure;
}

//  =========================================================================================
void Profiler::DestroyMeasurementTreeRecurssive()
{	
	bool isTreeTraversed = false;

	delete(m_measurementHistory[m_frameIndex]);
	m_measurementHistory[m_frameIndex] = nullptr;	
}

//  =========================================================================================
void Profiler::PrintHistory()
{
	int currentIndex = m_frameIndex;

	for (int historyIndex = 0; historyIndex < MAX_HISTORY_COUNT; ++historyIndex)
	{
		int actualIndex = Modulus(currentIndex, MAX_HISTORY_COUNT);
		ProfileMeasurement* measurement = m_measurementHistory[actualIndex];
		
		if (measurement != nullptr)
			measurement->PrintTree();
		else
			return; //if we ever have a null pointer we know we reached the end of the list. (only happens if we arent at MAX_HISTORY_COUNT)

		currentIndex = Modulus(currentIndex - 1, MAX_HISTORY_COUNT);
		measurement = nullptr;
	}
}

//  =========================================================================================
ProfileMeasurement* Profiler::ProfileGetPreviousFrame(int skipCount)
{
	int returnIndex = 0;

	//ensure skipcount is positive and 
	skipCount = ClampInt(skipCount, 0, MAX_HISTORY_COUNT);

	//add one to skip count so we go back at least one record
	skipCount++; 

	//if my skipcount is less than zero subtract skip count from MAX_HISTORY_COUNT
	if (m_frameIndex - skipCount < 0)
		returnIndex = MAX_HISTORY_COUNT - skipCount;
	else
		returnIndex = m_frameIndex - skipCount;

	//return measurement history at proper skipped index
	return m_measurementHistory[returnIndex];
}

//  =========================================================================================
bool Profiler::ProfilerGetPreviousFrames(std::vector<ProfileMeasurement*>& history, int numFrames)
{
	int currentIndex = m_frameIndex;

	for (int historyIndex = 0; historyIndex < numFrames; ++historyIndex)
	{
		int actualIndex = Modulus(currentIndex, MAX_HISTORY_COUNT);

		history.push_back(m_measurementHistory[actualIndex]);

		currentIndex = Modulus(currentIndex - 1, MAX_HISTORY_COUNT);
	}

	return true;
}

//  =========================================================================================
void Profiler::MarkFrame()
{
	// move stack into measurementHistory
	if (m_stack != nullptr)
	{
		DestroyMeasurementTreeRecurssive();				

		m_measurementHistory[m_frameIndex] = m_stack;
		Pop();

		GUARANTEE_OR_DIE(m_stack == nullptr, Stringf("Profiler: Invalid mark frame. Stack is not empty %s", m_stack->m_id)); //means I pushed somewhere without popping if 
	}

	// handle pause and resume states 
	if (g_isProfilerPausing)
	{
		g_isProfilerPausing = false;
		g_isProfilerPaused = true;
	}
	if (g_isProfilerResuming)
	{
		g_isProfilerResuming = false;
		g_isProfilerPaused = false;
	}

	Push("frame");

	//increase frame index
	m_frameIndex = Modulus((m_frameIndex + 1), MAX_HISTORY_COUNT);
}

//  =========================================================================================
void Profiler::Push(const char* id)
{
	if (!g_isProfilerPaused)
	{
		ProfileMeasurement* measure = CreateMeasurement(id);

		if(m_stack == nullptr)
			m_stack = measure;
		else
		{
			measure->SetParent(m_stack);
			m_stack->AddChild(measure);

			//add the new measurement to the top of the tree structure
			m_stack = measure;	
		}

		measure = nullptr;
	}	
}

//  =========================================================================================
void Profiler::Pop()
{
	if (!g_isProfilerPaused)
	{
		//if this ever happens we popped too many times
		GUARANTEE_OR_DIE(m_stack != nullptr, "Profiler: Popped too many times - stack is empty"); 																											  

		//sets the end time for this interval's time
		m_stack->Finish();			
		m_stack = m_stack->m_parent;	
	}
}

//  =========================================================================================
void Profiler::PauseProfiler()
{
	//when the frame is completed in the next MarkFrame(), the profiler system will be pause.
	g_isProfilerPausing = true;
}

//  =========================================================================================
bool Profiler::IsPaused()
{
	return g_isProfilerPaused;
}

//  =========================================================================================
void Profiler::ResumeProfiler()
{
	//when the frame is completed in the next MarkFrame(), the profiler system will be unpause and allow captures again.
	g_isProfilerResuming = true;
}


//  =========================================================================================
// console commands =============================================================================
//  =========================================================================================
void Pause(Command & cmd)
{
	Profiler::GetInstance()->PauseProfiler();
}


//  =========================================================================================
void Resume(Command & cmd)
{
	Profiler::GetInstance()->ResumeProfiler();
}

//  =========================================================================================
void LogHistory(Command & cmd)
{
	Profiler::GetInstance()->PrintHistory();
}

//  =========================================================================================
void Report(Command& cmd)
{
	ProfileMeasurement* measurement = Profiler::GetInstance()->ProfileGetPreviousFrame();

	ProfilerReport report;

	std::string typeParam = cmd.GetNextString();
	if (typeParam == "flat")
	{
		report.GenerateReportTreeFromFrame(measurement);
		report.PrintReportToDevConsole();
	}
	else if (typeParam == "tree")
	{
		report.GenerateReportTreeFromFrame(measurement);
		report.PrintReportToDevConsole();
	}		
	else
	{
		DevConsolePrintf(Rgba::RED, "INVALID REPORT TYPE - MUST HAVE EITHER 'tree' or 'flat' parameter");
	}
		
	measurement = nullptr;
}
#else

//  =========================================================================================
ProfileMeasurement* Profiler::CreateMeasurement(const char* id)
{
	UNUSED(id);
	return nullptr;
}
void Profiler::DestroyMeasurementTreeRecurssive(){}
void Profiler::PrintHistory(){}

//  =========================================================================================
ProfileMeasurement* Profiler::ProfileGetPreviousFrame(int skipCount)
{
	UNUSED(skipCount);
	return nullptr;
}

//  =========================================================================================
bool Profiler::ProfilerGetPreviousFrames(std::vector<ProfileMeasurement*>& history, int numFrames)
{
	UNUSED(history);
	UNUSED(numFrames);
	return false;
}


//  =========================================================================================
void Profiler::MarkFrame(){}

//  =========================================================================================
void Profiler::Push(const char* id){UNUSED(id);}

//  =========================================================================================
void Profiler::Pop(){}

//  =========================================================================================
void Profiler::PauseProfiler(){}

//  =========================================================================================
void Profiler::ResumeProfiler(){}

//  =========================================================================================
bool Profiler::IsPaused(){return false;}

//  =========================================================================================
void Pause(Command& cmd)
{
	UNUSED(cmd);
	DevConsolePrintf("Profiler disabled\n");
}

//  =========================================================================================
void Resume(Command& cmd)
{
	UNUSED(cmd);
	DevConsolePrintf("Profiler disabled\n");
}

//  =========================================================================================
void LogHistory(Command& cmd)
{
	UNUSED(cmd);
	DevConsolePrintf("Profiler disabled\n");
}

//  =========================================================================================
void Report(Command& cmd)
{
	UNUSED(cmd);
	DevConsolePrintf("Profiler disabled\n");
	
}
#endif