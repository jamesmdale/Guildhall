#include "Game\TankUI.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\Menu\PlayingState.hpp"


TankUI::TankUI()
{

}


TankUI::~TankUI()
{
}

void TankUI::Initialize()
{
	RefreshRenderables();
}

void TankUI::Update(float deltaSeconds)
{
	if (m_tankHealthThisFrame != m_tankHealthLastFrame || m_numEnemiesThisFrame != m_numEnemiesLastFrame || !m_isPlayerAlive) 
	{
		RefreshRenderables();
	}

	//copy any members over
	m_tankHealthLastFrame = m_tankHealthThisFrame;
	m_numEnemiesLastFrame = m_numEnemiesThisFrame;
}

void TankUI::RefreshRenderables()
{
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->RemoveRenderable(m_renderables[renderableIndex]);

		if (m_renderables[renderableIndex] != nullptr)
		{
			delete(m_renderables[renderableIndex]);
			m_renderables[renderableIndex] = nullptr;
		}
	}
	m_renderables.clear();

	// create card layout =============================================================================
	Renderer* theRenderer = Renderer::GetInstance();
	Window* clientWindow = Window::GetInstance();
	MeshBuilder mb;

	Vector2 dimensions = Vector2(clientWindow->GetClientDimensions().x - 10.f, clientWindow->GetClientDimensions().y - 10.f);
	AABB2 healthBounds = AABB2(Vector2(dimensions - Vector2(200.f, 200.f)), dimensions);
	AABB2 numEnemiesBounds = AABB2(Vector2(healthBounds.mins.x, healthBounds.mins.y - 40.f), Vector2(healthBounds.maxs.x, healthBounds.mins.y));

	Renderable2D* tankUiRenderable = new Renderable2D();	

	//green is full health - transitions to red
	float percentageAlive = ClampFloatZeroToOne((float)m_tankHealthThisFrame/100.f);	
	Rgba color = Rgba(1.f - percentageAlive, percentageAlive, 0.f, 1.f);

	mb.CreateText2DInAABB2(healthBounds.GetCenter(), healthBounds.GetDimensions(), 4.f / 3.f, Stringf("%s: %i/100", "Health", m_tankHealthThisFrame), color); //health
	mb.CreateText2DInAABB2(numEnemiesBounds.GetCenter(), numEnemiesBounds.GetDimensions(), 4.f / 3.f, Stringf("%s: %i", "Swarmers Alive: ", m_numEnemiesThisFrame), Rgba::WHITE); //numenemies
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));
	tankUiRenderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), materialInstance);

	
	//if player is dead, we need to show overlay
	if (m_isPlayerAlive == false)
	{
		PlayingState* gameState = (PlayingState*)g_currentState;
		int timeRemainingBeforeRespawn = g_respawnTimer- (int)gameState->m_respawnTimer->GetElapsedTimeInSeconds();

		//add overlay
		mb.CreateQuad2D(clientWindow->GetClientWindow(), Rgba::WHITE);
		materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
		materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::LIGHT_RED_TRANSPARENT));
		tankUiRenderable->AddRenderableData(3, mb.CreateMesh<VertexPCU>(), materialInstance);

		mb.CreateText2DInAABB2(clientWindow->GetCenterOfClientWindow(), Vector2(500.f, 500.f), 4.f / 3.f, "YOU ARE VERY DEAD", Rgba::WHITE); //numenemies
		Vector2 textCenter = clientWindow->GetCenterOfClientWindow() - Vector2(0.f, 200);
		if (!gameState->m_respawnTimer->HasElapsed())
		{
			float roundedFloat = RoundDownToDecimalPlace(timeRemainingBeforeRespawn, 100);
			mb.CreateText2DInAABB2(textCenter, Vector2(500.f, 500.f), 4.f / 3.f, Stringf("Time until respawn: %f", roundedFloat), Rgba::WHITE); //numenemies
		}
		else
		{
			mb.CreateText2DInAABB2(textCenter, Vector2(500.f, 500.f), 4.f / 3.f, "Space to respawn", Rgba::WHITE); //numenemies

		}
		materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
		materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));
		tankUiRenderable->AddRenderableData(4, mb.CreateMesh<VertexPCU>(), materialInstance);
	}

	materialInstance = nullptr;

	m_renderables.push_back(tankUiRenderable);
	m_renderScene->AddRenderable(tankUiRenderable);

	clientWindow = nullptr;
	theRenderer = nullptr;
}
