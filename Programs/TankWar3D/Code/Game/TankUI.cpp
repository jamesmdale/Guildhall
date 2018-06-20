#include "Game\TankUI.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\StringUtils.hpp"


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
	if (m_tankHealthThisFrame != m_tankHealthLastFrame) 
	{
		RefreshRenderables();
	}

	//copy any members over
	m_tankHealthLastFrame = m_tankHealthThisFrame;
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

	Vector2 dimensions = Vector2(clientWindow->GetClientDimensions().x - 50.f, clientWindow->GetClientDimensions().y - 50.f);
	AABB2 uiBounds = AABB2(Vector2(dimensions - Vector2(200.f, 200.f)), dimensions);

	Renderable2D* tankUiRenderable = new Renderable2D();
	

	//green is full health - transitions to red
	float percentageAlive = ClampFloatZeroToOne((float)m_tankHealthThisFrame/100.f);	
	Rgba color = Rgba(1.f - percentageAlive, percentageAlive, 0.f, 1.f);

	mb.CreateText2DInAABB2(uiBounds.GetCenter(), uiBounds.GetDimensions(), 4.f / 3.f, Stringf("%s: %i/100", "Health", m_tankHealthThisFrame), color); //name
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));
	tankUiRenderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), materialInstance);

	m_renderables.push_back(tankUiRenderable);

	m_renderScene->AddRenderable(tankUiRenderable);

	clientWindow = nullptr;
	theRenderer = nullptr;
}
