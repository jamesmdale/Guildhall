#include "Game\GameStates\PlayingState.hpp"
#include "Game\PointOfInterest.hpp"
#include "Game\Agent.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Math\IntVector2.hpp"
#include <map>
#include <string>

float ORTHO_MAX = 0.f;
float ORTHO_MIN = 0.f;
float g_orthoZoom = 0.f;

float ZOOM_RATE = 100.f;


//  =============================================================================
PlayingState::~PlayingState()
{
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
	m_uiCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_uiCamera->SetView(Matrix44::IDENTITY);

	g_orthoZoom = Window::GetInstance()->GetClientHeight();
	ORTHO_MAX = g_orthoZoom * 5.f;
	ORTHO_MIN = g_orthoZoom;

	// map creation
	MapDefinition* definition = MapDefinition::s_definitions["Grass"];
	m_map = new Map(definition, "TestMap", m_renderScene2D);	

	//set globals based on map
	g_maxCoordinateDistanceSquared = GetDistanceSquared(IntVector2::ZERO, m_map->GetDimensions());
	
	for (int armoryIndex = 0; armoryIndex < 2; ++armoryIndex)
	{
		//add random point of interest
		PointOfInterest* poiLocation = m_map->GeneratePointOfInterest(ARMORY_POI_TYPE);
		poiLocation->m_map = m_map;

		m_map->m_pointsOfInterest.push_back(poiLocation);
		poiLocation = nullptr;
	}
	
	for (int lumberyardIndex = 0; lumberyardIndex < 2; ++lumberyardIndex)
	{
		//add random point of interest
		PointOfInterest* poiLocation = m_map->GeneratePointOfInterest(LUMBERYARD_POI_TYPE);
		poiLocation->m_map = m_map;

		m_map->m_pointsOfInterest.push_back(poiLocation);
		poiLocation = nullptr;
	}

	//test agent
	
	IntVector2 dimensions = m_map->GetDimensions();

	AABB2 mapBounds = AABB2(Vector2::ZERO, Vector2(dimensions));
	
	for (int i = 0; i < 10; ++i)
	{
		IsoSpriteAnimSet* animSet = nullptr;
		std::map<std::string, IsoSpriteAnimSetDefinition*>::iterator spriteDefIterator = IsoSpriteAnimSetDefinition::s_isoSpriteAnimSetDefinitions.find("agent");
		if (spriteDefIterator != IsoSpriteAnimSetDefinition::s_isoSpriteAnimSetDefinitions.end())
		{
			animSet = new IsoSpriteAnimSet(spriteDefIterator->second);
		}

		Vector2 randomStartingLocation = m_map->GetRandomNonBlockedPositionInMapBounds();
		Agent* agent = new Agent(randomStartingLocation, animSet, m_map);
		m_map->m_agents.push_back(agent);

		animSet = nullptr;
		agent = nullptr;
	}
	
	//re-adjust camera center
	Vector2 mapCenter = -1.f * m_map->m_mapWorldBounds.GetCenter();
	m_camera->SetPosition(Vector3(mapCenter.x, mapCenter.y, 0.f));

	//cleanup
	definition = nullptr;	
	theRenderer = nullptr;
	theWindow = nullptr;
}

//  =============================================================================
void PlayingState::Update(float deltaSeconds)
{ 
	m_map->Update(deltaSeconds);
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

	m_map->Render();

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
		m_camera->Translate(Vector3(Vector2::DOWN * 100.f * deltaSeconds));
	}
	if (theInput->IsKeyPressed(theInput->KEYBOARD_UP_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::UP * 100.f * deltaSeconds));
	}
	if (theInput->IsKeyPressed(theInput->KEYBOARD_RIGHT_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::RIGHT * 100.f * deltaSeconds));
	}
	if (theInput->IsKeyPressed(theInput->KEYBOARD_LEFT_ARROW))
	{
		m_camera->Translate(Vector3(Vector2::LEFT * 100.f * deltaSeconds));
	}

	if (theInput->IsKeyPressed(theInput->KEYBOARD_PAGEUP))
	{
		g_orthoZoom -= deltaSeconds * ZOOM_RATE;
		if(g_orthoZoom < ORTHO_MIN)
		{
			g_orthoZoom = ORTHO_MIN;
		}
		m_camera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -1000.f, 1000.f);
	}

	if (theInput->IsKeyPressed(theInput->KEYBOARD_PAGEDOWN))
	{
		g_orthoZoom += deltaSeconds * ZOOM_RATE;
		if(g_orthoZoom > ORTHO_MAX)
		{
			g_orthoZoom = ORTHO_MAX;
		}

		m_camera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -1000.f, 1000.f);
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_R))
	{
		g_orthoZoom = Window::GetInstance()->GetClientHeight();
		m_camera->SetProjectionOrtho(g_orthoZoom, CLIENT_ASPECT, -1000.f, 1000.f);
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_T))
	{
		g_showBlockedTileData = !g_showBlockedTileData;
	}

	return deltaSeconds; //new deltaSeconds
}

//  =========================================================================================
void PlayingState::RenderUI()
{
	TODO("Render UI");

	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_uiCamera);


	//theRenderer->DrawText2DCentered()

	////cleanup
	//theRenderer->SetCamera(m_camera);
	//theRenderer = nullptr;
}


