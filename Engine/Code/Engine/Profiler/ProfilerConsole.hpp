#pragma once
#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Core\Widget.hpp"
#include <vector>
#include <string>
#include <map>

class ProfilerConsole
{
public:
	ProfilerConsole();
	~ProfilerConsole();

	void Initialize();
	void UpdateFromInput();
	void Update();
	void PreRender();
	void Render();

	Widget* m_base = nullptr;
	Widget* m_reportContent = nullptr;
	//Widget* m_reportGraph = nullptr;

	std::vector<ProfilerReport*> m_reports;
};

