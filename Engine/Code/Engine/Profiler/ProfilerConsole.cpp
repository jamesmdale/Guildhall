#include "Engine\Profiler\ProfilerConsole.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Profiler\ProfilerReport.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Profiler\Profiler.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Time\Time.hpp"

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
	//register commands	
	CommandRegister("profiler", CommandRegistration(Open, ": Open/close profiler console", "Profiler toggled!"));

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
	m_profilerRenderScene->AddCamera(m_profilerCamera);

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
	delete(m_activeReport);
	m_activeReport = new ProfilerReport();

	m_activeReport->GenerateReportTreeFromFrame(Profiler::GetInstance()->ProfileGetPreviousFrame());

	RefreshDynamicWidgets();
}

void ProfilerConsole::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_profilerCamera);

	//always do this first at the beginning of the frame's render
	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(ALWAYS_DEPTH_TYPE, true);	

	//render from forward rendering path
	m_profilerRenderingPath->Render(m_profilerRenderScene);

	theRenderer = nullptr;
}

void ProfilerConsole::PreRender()
{
	m_base->PreRender();
	m_reportContent->PreRender();
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

	//cleanup
	theWindow = nullptr;
	theRenderer = nullptr;
}

void ProfilerConsole::RefreshDynamicWidgets()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder mb;

	// report content data =============================================================================
	m_reportContent->DeleteRenderables();

	Renderable2D* reportContentRenderable = new Renderable2D();

	AABB2 contentQuad = AABB2(theWindow->GetClientWindow(), Vector2(0.01f, 0.01f), Vector2(0.99f, 0.7f));

	mb.CreateQuad2D(contentQuad, Rgba::LIGHT_BLUE_TRANSPARENT);

	reportContentRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));

	std::vector<std::string>* reportEntries = m_activeReport->GetEntriesAsFormattedStrings();
	for (int entryIndex = 0; entryIndex < (int)reportEntries->size(); ++entryIndex)
	{
		AABB2 textQuad;
		textQuad.mins = Vector2(contentQuad.mins.x + PROFILER_TEXT_WIDTH_PADDING, contentQuad.maxs.y - ((entryIndex + 1) * PROFILER_TEXT_CELL_HEIGHT) - PROFILER_TEXT_HEIGHT_PADDING);
		textQuad.maxs = Vector2(contentQuad.maxs.x - PROFILER_TEXT_WIDTH_PADDING, contentQuad.maxs.y - (entryIndex * PROFILER_TEXT_CELL_HEIGHT) + PROFILER_TEXT_HEIGHT_PADDING);

		mb.CreateText2DFromPoint(textQuad.mins, PROFILER_TEXT_CELL_HEIGHT, 1.f, reportEntries->at(entryIndex), Rgba::WHITE);
	}

	reportContentRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("text")));

	// fps counter content =============================================================================

	//create fps box
	AABB2 fpsQuad = AABB2(theWindow->GetClientWindow(), Vector2(0.01f, 0.85f), Vector2(0.3f, 0.99f));
	mb.CreateQuad2D(fpsQuad, Rgba::LIGHT_BLUE_TRANSPARENT);
	reportContentRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));

	std::string fpsAsString = Stringf("FPS: %-4.2f", GetMasterFPS());

	mb.CreateText2DInAABB2(fpsQuad.GetCenter(), fpsQuad.GetDimensions() - Vector2(20.f, 20.f), 4.f/3.f, fpsAsString, Rgba::WHITE);
	reportContentRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("text")));

	//create frame time box
	AABB2 frameQuad = AABB2(theWindow->GetClientWindow(), Vector2(0.01f, 0.71f), Vector2(0.3f, 0.84f));
	mb.CreateQuad2D(frameQuad, Rgba::LIGHT_BLUE_TRANSPARENT);
	reportContentRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));

	std::string frameAsString = Stringf("FRAME: %-4.5f", GetMasterDeltaSeconds());

	mb.CreateText2DInAABB2(frameQuad.GetCenter(), frameQuad.GetDimensions() - Vector2(20.f, 20.f), 4.f/3.f, frameAsString, Rgba::WHITE);
	reportContentRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("text")));

	// graph content =============================================================================

	AABB2 graphQuad = AABB2(theWindow->GetClientWindow(), Vector2(0.31f, 0.72f), Vector2(0.99f, 0.99f));
	mb.CreateQuad2D(graphQuad, Rgba::LIGHT_BLUE_TRANSPARENT);
	reportContentRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));

	// add renderable to widget and scene =============================================================================
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


// console commands =============================================================================
void Open(Command & cmd)
{
	ProfilerConsole::GetInstance()->m_isProfilerConsoleOpen = !ProfilerConsole::GetInstance()->m_isProfilerConsoleOpen;
}