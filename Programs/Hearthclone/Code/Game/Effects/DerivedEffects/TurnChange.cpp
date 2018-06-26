#include "Game\Effects\DerivedEffects\TurnChange.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"


TurnChange::TurnChange(ePlayerType toPlayerType, float totalEffectTime, RenderScene2D* renderScene)
{
	m_toPlayerType = toPlayerType;
	m_totalEffectTime = totalEffectTime;
	m_renderScene = renderScene;
}

TurnChange::~TurnChange() 
{
	delete(m_renderable);
	m_renderable = nullptr;

	m_renderScene = nullptr;
}

void TurnChange::InitializeTurnRenderable()
{
	m_renderable = new Renderable2D();

	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();
	MeshBuilder mb;

	//get size of text
	Vector2 windowCenter = theWindow->GetCenterOfClientWindow();
	Vector2 textBoxSize = theWindow->GetClientDimensions() * 0.25f;

	std::string text = "";
	if(m_toPlayerType == SELF_PLAYER_TYPE)
		text = "YOUR TURN";
	else
		text = "ENEMY TURN";

	mb.CreateText2DInAABB2(windowCenter, textBoxSize, 4.f / 3.f, text, Rgba::WHITE);	
	Material* textInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	textInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	m_renderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), textInstance);
	m_renderable->m_widgetSortLayer = 999;
	//m_renderScene->AddRenderable(m_renderable);

	textInstance = nullptr;
	theWindow = nullptr;
	theRenderer = nullptr;
}

void TurnChange::Update(float deltaSeconds)
{
	if (m_stopWatch == nullptr)
	{
		m_stopWatch = new Stopwatch(GetMasterClock());
		m_stopWatch->SetTimer(m_totalEffectTime);
		InitializeTurnRenderable();
	}

	// finished logic =========================================================================================
	if (m_stopWatch->HasElapsed())
	{
		m_isComplete = true;
	}
}