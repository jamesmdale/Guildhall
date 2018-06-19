#include "Game\Entity\Minion.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"



Minion::Minion()
{
}



Minion::Minion(const Card* fromCard)
{
	UpdateSortLayer(g_defaultCardSortLayer);

	m_cardReference = fromCard;
	m_controller = fromCard->m_controller;
	m_health = fromCard->m_health;
	m_attack = fromCard->m_attack;
	m_tags = fromCard->m_tags;
	
	m_minionImage = fromCard->m_cardImage;
}

Minion::~Minion()
{
	m_cardReference = nullptr;
	m_minionLayoutImage = nullptr;
	m_minionImage = nullptr;
}

void Minion::Initialize()
{
	UpdateSortLayer(g_defaultCardSortLayer);
	RefreshRenderables();
}

void Minion::Update(float deltaSeconds)
{
	if (m_isPositionLocked == false)
	{
		if (m_isInputPriority)
		{
			Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetMouseClientPosition();
			mousePosition = Vector2(ClampFloat(mousePosition.x, 0.f, Window::GetInstance()->GetClientWidth()), ClampFloat(mousePosition.y, 0.f, Window::GetInstance()->GetClientHeight()));
			m_transform2D->SetLocalPosition(mousePosition);
		}		
	}
	else
	{
		m_transform2D->SetLocalPosition(m_lockPosition);
	}
}

void Minion::RefreshRenderables()
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

	//create board layout
	Renderer* theRenderer = Renderer::GetInstance();
	Window* clientWindow = Window::GetInstance();
	MeshBuilder mb;

	float minionAspectRatio = 316.f/206.f;

	float minionHeight = g_cardPercentageOfClientWindowHeight * clientWindow->GetClientHeight();
	float minionWidth = minionHeight * minionAspectRatio;

	m_dimensionsInPixels = Vector2(minionWidth, minionHeight);
	Vector2 minionBottomRight = Vector2(Vector2::ZERO.x - (minionWidth * 0.5f), Vector2::ZERO.y - (minionHeight * 0.5f));

	Renderable2D* minionRenderable = new Renderable2D();

	//create card base image

	mb.CreateQuad2D(minionBottomRight + (m_dimensionsInPixels * g_minionImageCenterRatio), m_dimensionsInPixels * g_cardImageDimensionsRatio, Rgba::WHITE);

	Material* minionMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	minionMaterial->SetTexture(0, m_minionImage);
	
	minionRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), minionMaterial);


	m_renderables.push_back(minionRenderable);

	// add renderables to scene =========================================================================================
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	clientWindow = nullptr;
	theRenderer = nullptr;
}

Vector2 Minion::GetMinionDimensions()
{
	return m_dimensionsInPixels;
}

void Minion::OnLeftClicked()
{
}

void Minion::OnRightClicked()
{
}
