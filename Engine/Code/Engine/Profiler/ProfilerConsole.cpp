#include "Engine\Profiler\ProfilerConsole.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Profiler\ProfilerReport.hpp"

static ProfilerConsole* g_theProfilerConsole = nullptr;

ProfilerConsole::ProfilerConsole()
{
}

ProfilerConsole* ProfilerConsole::GetInstance()
{
	return g_theProfilerConsole;
}

ProfilerConsole* ProfilerConsole::CreateInstance()
{
	if (g_theProfilerConsole == nullptr) 
	{
		g_theProfilerConsole = new ProfilerConsole(); 
	}
	return g_theProfilerConsole; 
}

ProfilerConsole::~ProfilerConsole()
{
	delete(m_base);
	m_base = nullptr;

	delete(m_reportContent);
	m_reportContent = nullptr;

	delete(m_activeReport);
	m_activeReport = nullptr;
}

void ProfilerConsole::Startup()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();

	//setup camera
	m_profilerCamera = new Camera();
	m_profilerCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_profilerCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_profilerCamera->SetView(Matrix44::IDENTITY);

	//setup rendering path
	m_profilerRenderingPath = new ForwardRenderingPath2D();

	//setup renderscene
	m_profilerRenderScene = new RenderScene2D();

	//create profiler console widgets
	CreateWidgets();

	//cleanup
	theWindow = nullptr;
	theRenderer = nullptr;
}

void ProfilerConsole::Shutdown()
{
}

void ProfilerConsole::UpdateFromInput()
{
}

void ProfilerConsole::Update()
{
}

void ProfilerConsole::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_profilerCamera);

	//always do this first at the beginning of the frame's render
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	//render from forward rendering path
	m_profilerRenderingPath->Render(m_profilerRenderScene);

	theRenderer = nullptr;
}

void ProfilerConsole::PreRender()
{
}


void ProfilerConsole::CreateWidgets()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();
	MeshBuilder mb;

	// create base widget =============================================================================
	m_base = new Widget();
	m_base->m_renderScene = m_profilerRenderScene;
	m_base->UpdateSortLayer(PROFILER_SORT_LAYER);

	Renderable2D* backgroundRenderable = new Renderable2D();

	//full screen quad
	mb.CreateQuad2D(AABB2(theWindow->GetClientWindow().mins, theWindow->GetClientWindow().maxs), Rgba::LIGHT_BLUE_TRANSPARENT);

	backgroundRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));

	m_base->AddRenderable(backgroundRenderable);
	m_profilerRenderScene->AddRenderable(backgroundRenderable);

	backgroundRenderable = nullptr;

	// create report content widget =============================================================================

	m_reportContent = new Widget();
	m_reportContent->m_renderScene = m_profilerRenderScene;
	m_reportContent->UpdateSortLayer(PROFILER_SORT_LAYER + 1);

	//RefreshDynamicWidgets();
	

	//cleanup
	theWindow = nullptr;
	theRenderer = nullptr;
}

void ProfilerConsole::RefreshDynamicWidgets()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder mb;

	// report content =============================================================================
	m_reportContent->DeleteRenderables();

	Renderable2D* reportContentRenderable = new Renderable2D();

	AABB2 contentQuad = AABB2(theWindow->GetClientWindow(), Vector2(0.05f, 0.05f), Vector2(0.95f, 0.7f));

	mb.CreateQuad2D(contentQuad, Rgba::LIGHT_BLUE_TRANSPARENT);

	reportContentRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));

	std::vector<std::string>* reportEntries = m_activeReport->GetEntriesAsFormattedStrings();
	for (int entryIndex = 0; entryIndex < (int)reportEntries->size(); ++entryIndex)
	{
		Vector2 vec2 = Vector2(contentQuad.mins.x, contentQuad.maxs.y);
		AABB2 textQuad;
		textQuad.mins = Vector2(contentQuad.mins.x + PROFILER_TEXT_WIDTH_PADDING, contentQuad.maxs.y - (entryIndex * PROFILER_TEXT_CELL_HEIGHT));
		textQuad.maxs = Vector2(contentQuad.maxs.x - PROFILER_TEXT_WIDTH_PADDING, contentQuad.maxs.y - (entryIndex * PROFILER_TEXT_CELL_HEIGHT));

		mb.CreateText2DFromPoint(textQuad.mins, PROFILER_TEXT_CELL_HEIGHT, 1.f, reportEntries->at(entryIndex), Rgba::WHITE);
	}

	//add renderable to widget and scene
	m_reportContent->AddRenderable(reportContentRenderable);
	m_profilerRenderScene->AddRenderable(reportContentRenderable);

	delete(reportEntries);
	reportEntries = nullptr;
	reportContentRenderable = nullptr;


	// report content =============================================================================

	//cleanup
	theWindow = nullptr;
	theRenderer = nullptr;
}


