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
	delete(m_turnChangeWidget);
	m_turnChangeWidget = nullptr;

	m_renderScene = nullptr;
}

void TurnChange::InitializeTurnRenderable()
{
	m_turnChangeWidget = new Widget();
	m_turnChangeWidget->m_renderScene = m_renderScene;

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

	Renderable2D* renderable = new Renderable2D();

	mb.CreateText2DInAABB2(windowCenter, textBoxSize, 4.f / 3.f, text, Rgba::WHITE);	
	Material* textInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	textInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	renderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), textInstance);
	renderable->m_widgetSortLayer = 999;

	m_turnChangeWidget->m_renderables.push_back(renderable);
	m_renderScene->AddRenderable(renderable);

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