#include "Engine\Profiler\ProfilerConsole.hpp"



ProfilerConsole::ProfilerConsole()
{
}


ProfilerConsole::~ProfilerConsole()
{
	delete(m_base);
	m_base = nullptr;

	delete(m_reportContent);
	m_reportContent = nullptr;
}

void ProfilerConsole::Initialize()
{

}

void ProfilerConsole::UpdateFromInput()
{
}

void ProfilerConsole::Update()
{
}

void ProfilerConsole::PreRender()
{
}

void ProfilerConsole::Render()
{
}
