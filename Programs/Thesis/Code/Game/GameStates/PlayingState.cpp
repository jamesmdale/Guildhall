#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\File\FIleHelpers.hpp"
#include <map>
#include <string>
#include "Game\GameStates\PlayingState.hpp"
#include "Game\PointOfInterest.hpp"
#include "Game\Agent.hpp"
#include "Game\Definitions\SimulationDefinition.hpp"
#include "Game\SimulationData.hpp"

float ORTHO_MAX = 0.f;
float ORTHO_MIN = 0.f;
float g_orthoZoom = 0.f;

float ZOOM_RATE = 5.f;

std::string simDataOutputDirectory = "";

//  =============================================================================
PlayingState::~PlayingState()
{
	delete(m_simulationTimer);
	m_simulationTimer = nullptr;

	//delete scene last
	delete(m_renderScene2D);
	m_renderScene2D = nullptr;		
}

//  =============================================================================
void PlayingState::Initialize()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	//setup cameras
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_uiCamera->SetOrtho(0.f, theWindow->GetClientWidth(), 0.f, theWindow->GetClientHeight(), -1.f, 1.f);
	m_uiCamera->SetView(Matrix44::IDENTITY);

	//set game camera
	m_camera = new Camera();
	m_camera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_camera->SetOrtho(0.f, 32.f, 0.f, 18.f, -1000.f, 1000.f);
	m_camera->SetView(Matrix44::IDENTITY);

	g_orthoZoom = 1.f;
	ORTHO_MAX = 20.f;
	ORTHO_MIN = 0.f;

	//register commands
	RegisterCommand("toggle_optimization", CommandRegistration(ToggleOptimized, ": Toggle blanket optimizations on and off", ""));

	std::string newFolderName = Stringf("SIMULATION_TEST_%s", GetCurrentDateTime().c_str());
	std::string newPath =  Stringf("%s%s", "Data\\ExportedSimulationData\\", newFolderName.c_str());
	
	bool success = CreateFolder(newPath.c_str());
	ASSERT_OR_DIE(success, Stringf("UNABLE TO CREATE FOLDER (%s)", newPath.c_str()).c_str());

	simDataOutputDirectory = Stringf("%s%s", newPath.c_str(), "\\");

	SimulationDefinition* definition = SimulationDefinition::s_simulationDefinitions[g_currentSimDefinitionIndex];
	InitializeSimulation(definition);

	//cleanup
	definition = nullptr;	
	theRenderer = nullptr;
	theWindow = nullptr;
}

//  =============================================================================
void PlayingState::Update(float deltaSeconds)
{ 
	m_map->Update(deltaSeconds);

	if (m_simulationTimer->ResetAndDecrementIfElapsed())
	{
		ExportSimulationData();

		g_currentSimDefinitionIndex++;

		if (g_currentSimDefinitionIndex < SimulationDefinition::s_simulationDefinitions.size())
		{
			ExportSimulationData();
			DestroyCurrentSimulation();

			SimulationDefinition* definition = SimulationDefinition::s_simulationDefinitions[g_currentSimDefinitionIndex];

			delete(g_generalSimulationData);
			g_generalSimulationData = nullptr;

			InitializeSimulation(definition);
		}
		else
		{
			g_isQuitting = true;
		}	
	}
}

//  =============================================================================
void PlayingState::PreRender()
{
}

//  =============================================================================
void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Game::GetInstance()->m_forwardRenderingPath2D->Render(m_renderScene2D);

	RenderGame();
	RenderUI();

	theRenderer = nullptr;
}

//  =============================================================================
void PlayingState::PostRender()
{
	m_map->DeleteDeadEntities();
}

//  =============================================================================
float PlayingState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();

	if (theInput->IsKeyPressed(theInput->KEYBOARD_DOWN_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::DOWN * 5.f * deltaSeconds));
	}
	if (theInput->IsKeyPressed(theInput->KEYBOARD_UP_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::UP * 5.f * deltaSeconds));
	}
	if (theInput->IsKeyPressed(theInput->KEYBOARD_RIGHT_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::RIGHT * 5.f * deltaSeconds));
	}
	if (theInput->IsKeyPressed(theInput->KEYBOARD_LEFT_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::LEFT * 5.f * deltaSeconds));
	}

	//if (theInput->IsKeyPressed(theInput->KEYBOARD_PAGEUP))
	//{
	//	g_orthoZoom -= deltaSeconds * ZOOM_RATE;
	//	if(g_orthoZoom < ORTHO_MIN)
	//	{
	//		g_orthoZoom = ORTHO_MIN;
	//	}
	//	m_camera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -1000.f, 1000.f);
	//}

	//if (theInput->IsKeyPressed(theInput->KEYBOARD_PAGEDOWN))
	//{
	//	g_orthoZoom += deltaSeconds * ZOOM_RATE;
	//	if(g_orthoZoom > ORTHO_MAX)
	//	{
	//		g_orthoZoom = ORTHO_MAX;
	//	}

	//	m_camera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -1000.f, 1000.f);
	//}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_R))
	{
		g_orthoZoom = Window::GetInstance()->GetClientHeight();
		m_camera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -1000.f, 1000.f);
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_T))
	{
		g_showBlockedTileData = !g_showBlockedTileData;	
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_I))
	{
		g_isIdShown = !g_isIdShown;
	}
	
	return deltaSeconds; //new deltaSeconds
}

//  =========================================================================================
void PlayingState::RenderGame()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_camera);
	m_map->Render();

	theRenderer = nullptr;
}

//  =========================================================================================
void PlayingState::RenderUI()
{
	//TODO("Render UI");

	//Window* theWindow = Window::GetInstance();
	//Renderer* theRenderer = Renderer::GetInstance();
	//MeshBuilder mb;
	//theRenderer->SetCamera(m_uiCamera);

	//theRenderer->ClearDepth(1.0f);
	//theRenderer->EnableDepth(ALWAYS_DEPTH_TYPE, true);

	//AABB2 fpsQuad = AABB2(theWindow->GetClientWindow(), Vector2(0.01f, 0.85f), Vector2(0.3f, 0.99f));
	//std::string fpsAsString = Stringf("FPS: %-4.2f", GetUnclampedFPS());

	//theRenderer->SetTexture(*theRenderer->CreateOrGetTexture("Data\Fonts\SquirrelFixedFont.png"));
	//theRenderer->SetShader(theRenderer->CreateOrGetShader("alpha"));
	//theRenderer->DrawText2DCentered(fpsQuad.GetCenter(), fpsAsString, 20.f, Rgba::WHITE, 1.f, theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));

	////cleanup
	//theRenderer->SetTexture(*theRenderer->CreateOrGetTexture("default"));
	//theRenderer->SetShader(theRenderer->CreateOrGetShader("default"));
	//theRenderer->SetCamera(m_camera);


	//theRenderer = nullptr;
	//theWindow = nullptr;
}

//  =============================================================================
void PlayingState::InitializeSimulation(SimulationDefinition* definition)
{
	//current sim definition
	g_currentSimulationDefinition = definition;
	InitializeSimulationData();

	//map creation
	m_map = new Map(definition, "TestMap", m_renderScene2D);	
	m_map->Initialize();
	m_map->m_gameState = this;

	//re-adjust camera center
	Vector2 mapCenter = -1.f * m_map->m_mapWorldBounds.GetCenter();
	m_camera->SetPosition(Vector3(mapCenter.x, mapCenter.y, 0.f));	

	m_simulationTimer = new Stopwatch(GetMasterClock());
	m_simulationTimer->SetTimer(g_generalSimulationData->m_simulationDefinitionReference->m_totalProcessingTimeInSeconds);
}

//  =============================================================================
void PlayingState::InitializeSimulationData()
{	
	g_generalSimulationData = new SimulationData();
	g_generalSimulationData->Initialize(g_currentSimulationDefinition);

	//action stack data
	g_processActionStackData = new SimulationData();
	g_processActionStackData->Initialize(g_currentSimulationDefinition);

	//action stack data
	g_updatePlanData = new SimulationData();
	g_updatePlanData->Initialize(g_currentSimulationDefinition);

	//action stack data
	g_agentUpdateData = new SimulationData();
	g_agentUpdateData->Initialize(g_currentSimulationDefinition);

	//action stack data
	g_pathingData = new SimulationData();
	g_pathingData->Initialize(g_currentSimulationDefinition);

	//action stack data
	g_copyPathData = new SimulationData();
	g_copyPathData->Initialize(g_currentSimulationDefinition);
}

//  =============================================================================
void PlayingState::DestroyCurrentSimulation()
{
	delete(g_generalSimulationData);
	g_generalSimulationData = nullptr;

	delete(g_processActionStackData);
	g_processActionStackData = nullptr;

	delete(g_updatePlanData);
	g_updatePlanData = nullptr;

	delete(g_agentUpdateData);
	g_agentUpdateData = nullptr;

	delete(g_copyPathData);
	g_copyPathData = nullptr;

	g_numUpdatePlanCalls = 0;
	g_numActionStackProcessCalls = 0;
	g_numAgentUpdateCalls = 0;
	g_numGetPathCalls = 0;
	g_numCopyPathCalls = 0;

	delete(m_map);
	m_map = nullptr;
}

//  =============================================================================
void PlayingState::ExportSimulationData()
{
	//std::string newFolderName = Stringf("SIMULATION_TEST_%s", GetCurrentDateTime().c_str());
	//std::string newPath =  Stringf("%s%s", "Data\\ExportedSimulationData\\", newFolderName);
	//CreateFolder(newPath.c_str());	

	std::string newFolder = Stringf("%s%s%i", simDataOutputDirectory.c_str(), "Simulation_Definition_", g_currentSimDefinitionIndex);
	CreateFolder(newFolder.c_str());

	std::string finalFilePath = Stringf("%s%s", newFolder.c_str(), "//");

	//general data
	g_generalSimulationData->ExportCSV(finalFilePath, "GeneralInfo.csv");

	//export action stack data
	g_processActionStackData->ExportCSV(finalFilePath, "ActionStackAverageTimes.csv");

	//export update plan data
	g_updatePlanData->ExportCSV(finalFilePath, "UpdatePlanAverageTimes.csv");

	//export stack data
	g_agentUpdateData->ExportCSV(finalFilePath, "AgentUpdateAverageTimes.csv");

	//export stack data
	g_pathingData->ExportCSV(finalFilePath, "PathingDataAverageTimes.csv");

	//action stack data
	g_copyPathData ->ExportCSV(finalFilePath, "CopyPathAverageTimes.csv");
}

// Commands =============================================================================
void ToggleOptimized(Command& cmd)
{
	Game* theGame = Game::GetInstance();

	//theGame->m_isOptimized = !theGame->m_isOptimized;

	//if (theGame->m_isOptimized)
	//{
	//	DevConsolePrintf(Rgba::YELLOW, "Game is optimized!");
	//}
	//else
	//{
	//	DevConsolePrintf(Rgba::YELLOW, "Game is NOT optimized!");
	//}

	theGame = nullptr;
}
