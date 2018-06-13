#include "Game\Entity\Card.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\Meshbuilder.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\StringUtils.hpp"


Vector2 cardDimensions = Vector2(0.f, 0.f);

Card::Card()
{
}

Card::Card(const CardDefinition* definition)
{
	m_definition = definition;
	m_cost = definition->m_cost;
	m_cardImage = Renderer::GetInstance()->CreateOrGetTexture(definition->m_imagePath);

	m_health = definition->m_health;
	m_attack = definition->m_attack;

	m_tags = definition->m_tags;
	m_text = definition->m_text;
	
	switch (m_definition->m_type)
	{
	case SPELL_TYPE:
		m_cardLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Simple.png");
		break;

	case MINION_TYPE:
		m_cardLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Everything.png");
		break;

	case WEAPON_TYPE:
		m_cardLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Everything.png");
		break;
	}
}


Card::~Card()
{
	m_definition = nullptr;
	m_cardImage = nullptr;
	m_cardLayoutImage = nullptr;
}

void Card::Initialize()
{
	RefreshCardRenderables();
	
	//other intialization data goes here.
}

void Card::RefreshCardRenderables()
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

	float cardHeight = g_cardPercentageOfClientWindowHeight * clientWindow->GetClientHeight();
	float cardWidth = cardHeight * g_cardAspectRatio;

	cardDimensions = Vector2(cardWidth, cardHeight);
	Vector2 cardBottomRight = Vector2(Vector2::ZERO.x - (cardWidth * 0.5f), Vector2::ZERO.y - (cardHeight * 0.5f));

	// create background renderable =========================================================================================
	Renderable2D* cardRenderableBackground = new Renderable2D();

	mb.CreateQuad2D(Vector2::ZERO, cardDimensions * 0.99, GetCardColorByClass(m_definition->m_class)); //make it slightly smaller
	cardRenderableBackground->AddMesh(mb.CreateMesh<VertexPCU>());

	cardRenderableBackground->SetMaterial(Material::Clone(theRenderer->CreateOrGetMaterial("default")));
	cardRenderableBackground->SetRender2DSortLayer(2);
	m_renderables.push_back(cardRenderableBackground);

	// create card template overlay renderable =========================================================================================
	Renderable2D* cardRenderableOverlay = new Renderable2D();

	mb.CreateQuad2D(Vector2::ZERO, cardDimensions, Rgba::WHITE);
	cardRenderableOverlay->AddMesh(mb.CreateMesh<VertexPCU>());

	cardRenderableOverlay->SetMaterial(Material::Clone(theRenderer->CreateOrGetMaterial("alpha")));
	cardRenderableOverlay->SetRender2DSortLayer(2);
	cardRenderableOverlay->GetMaterial()->SetTexture(0, m_cardLayoutImage);
	m_renderables.push_back(cardRenderableOverlay);


	// create card image renderable =========================================================================================

	Renderable2D* cardRenderableImage = new Renderable2D();

	mb.CreateQuad2D(cardBottomRight + (cardDimensions * g_cardImageCenterRatio), cardDimensions * g_cardImageDimensionsRatio, Rgba::WHITE);
	cardRenderableImage->AddMesh(mb.CreateMesh<VertexPCU>());

	cardRenderableImage->SetMaterial(Material::Clone(theRenderer->CreateOrGetMaterial("default")));
	cardRenderableImage->SetRender2DSortLayer(2);
	cardRenderableImage->GetMaterial()->SetTexture(0, m_cardImage);
	m_renderables.push_back(cardRenderableImage);

	// create card image renderable =========================================================================================
	Renderable2D* cardTextRenderable = new Renderable2D();

	mb.CreateText2DInAABB2(cardBottomRight + (cardDimensions * g_cardNameCenterRatio), cardDimensions * g_cardNameDimensionsRatio, 4.f / 3.f, m_name, Rgba::WHITE); //name
	mb.CreateText2DInAABB2(cardBottomRight + (cardDimensions * g_cardManaCenterRatio), cardDimensions * g_cardManaDimensionsRatio, 1.f, Stringf("%i", m_cost), Rgba::WHITE); //mana
	mb.CreateText2DInAABB2(cardBottomRight + (cardDimensions * g_cardAttackCenterRatio), cardDimensions * g_cardAttackDimensionsRatio, 1.f, Stringf("%i", m_attack), Rgba::WHITE); //attack
	mb.CreateText2DInAABB2(cardBottomRight + (cardDimensions * g_cardHealthCenterRatio), cardDimensions * g_cardHealthDimensionsRatio, 1.f, Stringf("%i", m_health), Rgba::WHITE); //health
	mb.CreateText2DInAABB2(cardBottomRight + (cardDimensions * g_cardTextCenterRatio), cardDimensions * g_cardTextDimensionsRatio, 4.f / 3.f, m_text, Rgba::WHITE); //Text
	cardTextRenderable->AddMesh(mb.CreateMesh<VertexPCU>());	

	cardTextRenderable->SetMaterial(Material::Clone(theRenderer->CreateOrGetMaterial("text")));
	cardTextRenderable->GetMaterial()->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	cardTextRenderable->SetRender2DSortLayer(3);
	m_renderables.push_back(cardTextRenderable);

	// add renderables to scene =========================================================================================
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	m_isRendering = true;

	clientWindow = nullptr;
	theRenderer = nullptr;
}

Vector2 Card::GetCardDimensions()
{
	return cardDimensions;
}
