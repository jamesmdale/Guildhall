#include "Game\GameStates\PlayingState.hpp"
#include "Game\PointOfInterest.hpp"
#include "Game\Agent.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Utility\AStar.hpp"
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
	
	//test agent
	IsoSpriteAnimSet* animSet = nullptr;
	std::map<std::string, IsoSpriteAnimSetDefinition*>::iterator spriteDefIterator = IsoSpriteAnimSetDefinition::s_isoSpriteAnimSetDefinitions.find("agent");
	if (spriteDefIterator != IsoSpriteAnimSetDefinition::s_isoSpriteAnimSetDefinitions.end())
	{
		animSet = new IsoSpriteAnimSet(spriteDefIterator->second);
	}
	IntVector2 dimensions = m_map->GetDimensions();

	AABB2 mapBounds = AABB2(Vector2::ZERO, Vector2(dimensions));
	Vector2 randomStartingLocation = m_map->GetRandomNonBlockedPositionInMapBounds();
	Agent* agent = new Agent(randomStartingLocation, animSet, m_map);
	m_agents.push_back(agent);

	//add random point of interest
	Vector2 randomPOILocation = m_map->GetRandomNonBlockedPositionInMapBounds();
	randomPOILocation.Floor();

	PointOfInterest* testLocation = new PointOfInterest(Vector2(randomPOILocation.x + 0.5f, randomPOILocation.y + 0.5f), g_tileSize);
	m_pointOfInterests.push_back(testLocation);
	
	//test for A*
	Grid<int>* mapGrid = m_map->GetAsGrid();
	std::vector<IntVector2> searchPath;
	IntVector2 startPos = m_map->GetTileCoordinateOfPosition(agent->m_position);
	IntVector2 endPos = IntVector2(m_map->GetTileCoordinateOfPosition(testLocation->m_position));
	bool isDestinationFound = false;

	//add the location
	isDestinationFound = AStarSearch(searchPath, *mapGrid, startPos, endPos);

	//re-adjust camera center
	Vector2 mapCenter = -1.f * m_map->m_mapWorldBounds.GetCenter();
	m_camera->SetPosition(Vector3(mapCenter.x, mapCenter.y, 0.f));

	m_agents[0]->m_currentPath = searchPath;

	//cleanup
	definition = nullptr;
	agent = nullptr;
	animSet = nullptr;
	theRenderer = nullptr;
}

//  =============================================================================
void PlayingState::Update(float deltaSeconds)
{ 
	for (int agentIndex = 0; agentIndex < (int)m_agents.size(); ++agentIndex)
	{
		m_agents[agentIndex]->Update(deltaSeconds);
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

	for (int tileIndex = 0; tileIndex < (int)m_map->m_tiles.size(); ++tileIndex)
	{
		m_map->m_tiles[tileIndex]->Render();
	}

	for (int pointOfInterestIndex = 0; pointOfInterestIndex < (int)m_pointOfInterests.size(); ++pointOfInterestIndex)
	{
		m_pointOfInterests[pointOfInterestIndex]->Render();
	}

	for (int agentIndex = 0; agentIndex < (int)m_agents.size(); ++agentIndex)
	{
		m_agents[agentIndex]->Render();
	}


	theRenderer = nullptr;
}

//  =============================================================================
void PlayingState::PostRender()
{
	//post render steps here.
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

//  =============================================================================
Widget* PlayingState::GetSelectedWidget(const std::vector<Widget*>& widgets)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();

	//create a vector of widgets that are under the mouse position.  We will sort according to layer to find selected widget
	Widget* selectedWidget = nullptr;

	//add each widget at the mouse cursor to 
	for (int widgetIndex = 0; widgetIndex < (int)widgets.size(); ++widgetIndex)
	{
		Widget* widget = widgets[widgetIndex];
		Vector2 position = widget->m_transform2D->GetWorldPosition();
		AABB2 widgetBounds = AABB2(position, widget->m_dimensionsInPixels.x, widget->m_dimensionsInPixels.y);

		if (widgetBounds.IsPointInside(mousePosition) == true)
		{
			if (selectedWidget == nullptr )
			{
				selectedWidget = widget;
			}
			else if (widget->GetSortLayer() > selectedWidget->GetSortLayer())
			{
				selectedWidget = widget;
			}
		}

		widget = nullptr;
	}

	// return 
	return selectedWidget;
}


