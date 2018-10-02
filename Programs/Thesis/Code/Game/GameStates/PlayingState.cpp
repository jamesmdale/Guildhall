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
	g_orthoZoom = Window::GetInstance()->GetClientHeight();
	ORTHO_MAX = g_orthoZoom * 5.f;
	ORTHO_MIN = g_orthoZoom;

	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	// map creation
	MapDefinition* definition = MapDefinition::s_definitions["Grass"];
	m_map = new Map(definition, "TestMap", m_renderScene2D);	
	
	//add random point of interest
	PointOfInterest* poiLocation = m_map->GeneratePointOfInterest(ARMORY_POI_TYPE);
	poiLocation->m_map = m_map;

	m_map->m_pointsOfInterest.push_back(poiLocation);

	//add random point of interest
	poiLocation = m_map->GeneratePointOfInterest(LUMBERYARD_POI_TYPE);
	poiLocation->m_map = m_map;

	m_map->m_pointsOfInterest.push_back(poiLocation);

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

	//Vector2 accessPosition = m_map->GetWorldPositionOfMapCoordinate(m_map->m_pointsOfInterest[0]->m_accessCoordinate);

	//test action for agent
	/*ActionData* data = new ActionData();
	data->m_action = ShootAction;
	data->m_finalGoalDestination = accessPosition;
	data->m_interactEntityId = m_map->m_pointsOfInterest[0]->m_id;*/

	//m_map->m_agents[0]->AddActionToStack(data);

	//cleanup
	definition = nullptr;	
	theRenderer = nullptr;
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

	return deltaSeconds; //new deltaSeconds
}


