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

float ZOOM_RATE = 5.f;

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

	// map creation
	MapDefinition* definition = MapDefinition::s_definitions["Grass"];
	m_map = new Map(definition, "TestMap", m_renderScene2D);	
	m_map->Initialize();
	m_map->m_gameState = this;
	
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
	TODO("Render UI");

	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_uiCamera);


	//theRenderer->DrawText2DCentered()

	////cleanup
	//theRenderer->SetCamera(m_camera);
	theRenderer = nullptr;
}


