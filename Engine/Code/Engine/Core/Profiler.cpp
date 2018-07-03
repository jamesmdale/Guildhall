#include "Engine\Core\Profiler.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"


static Profiler* g_theProfiler = nullptr;


Profiler::Profiler()
{
}

Profiler::~Profiler()
{
}

Profiler* Profiler::CreateInstance()
{
	if (g_theProfiler == nullptr)
	{
		g_theProfiler = new Profiler();
	}

	return g_theProfiler;
}

Profiler* Profiler::GetInstance()
{
	return g_theProfiler;
}

// utility methods =========================================================================================


ProfileMeasurement* Profiler::CreateMeasurement(const char* id)
{
	ProfileMeasurement* measure = new ProfileMeasurement();

	strncpy_s(measure->m_id, id, 64);
	measure->m_startTime = GetMasterClock()->GetLastHPC();

	return measure;
}

void Profiler::MarkFrame()
{
	if (m_stack != nullptr)
	{
		if (m_previousStack != nullptr)
		//	DestroyMeasurementTreeRecurssive(m_previousStack);

		m_previousStack = m_stack;
		Pop();

		//GUARANTEE_OR_DIE(m_stack == nullptr, Stringf("Profiler: Invalid mark frame. Stack is not empty %s", m_stack->m_id); //means I pushed somewhere without popping if 
	}

	Push("frame");
}

void Profiler::Push(const char* id)
{
	ProfileMeasurement* measure = CreateMeasurement(id);

	if(m_stack == nullptr)
		m_stack = measure;
	else
	{
		measure->SetParent(m_stack);
		m_stack->AddChild(measure);

		m_stack = measure;	//add the new measurement to the top of the tree structure
	}
}

void Profiler::Pop()
{
	//GUARANTEE_OR_DIE(m_stack != nullptr, "Profiler: Popped too many times - stack is empty"); //if this ever happens we popped too many times																											  

	//m_stack->Finish();	//sets the end time for this interval's time
	m_stack = m_stack->m_parent;
	
}