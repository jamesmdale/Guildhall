#pragma once
#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Core\Widget.hpp"
#include <vector>
#include <string>
#include <map>
#include "Engine\Renderer\ForwardRenderingPath2D.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Engine\Camera\Camera.hpp"

class ProfilerConsole
{
public:
	ProfilerConsole();
	~ProfilerConsole();

	void Startup();
	void Shutdown();

	void UpdateFromInput();
	void Update();
	void Render();
	void PreRender();

	void CreateWidgets();
	void RefreshDynamicWidgets();

	bool IsOpen(){return m_isProfilerConsoleOpen;}

public:
	static ProfilerConsole* GetInstance();
	static ProfilerConsole* CreateInstance();

public:

	bool m_doesHaveInputPriority = false;
	bool m_isProfilerConsoleOpen = false;

	//widgets
	Widget* m_base = nullptr;
	Widget* m_reportContent = nullptr;
	//Widget* m_reportGraph = nullptr;

	ProfilerReport* m_activeReport = nullptr;

	//rendering members
	RenderScene2D* m_profilerRenderScene = nullptr;
	ForwardRenderingPath2D* m_profilerRenderingPath = nullptr;
	Camera* m_profilerCamera = nullptr;
};

