#include "Game\Entity\Minion.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Game\Board.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Effects\DerivedEffects\TargetEffect.hpp"
#include "Engine\Core\StringUtils.hpp"

Minion::Minion()
{
	m_characterId = Character::GenerateNewCharacterId();
	UpdateSortLayer(g_defaultCardSortLayer);

	AddTag("minion");
}

Minion::Minion(Card* fromCard)
{
	m_characterId = Character::GenerateNewCharacterId();
	UpdateSortLayer(g_defaultCardSortLayer);

	m_cardReference = fromCard;
	m_controller = fromCard->m_controller;
	m_health = fromCard->m_health;
	m_attack = fromCard->m_attack;
	m_tags = fromCard->m_tags;
	
	m_minionImage = fromCard->m_cardImage;

	m_minionLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Image_Circle_Drop_Frame.png");

	AddTag("minion");
}

Minion::~Minion()
{
	m_cardReference = nullptr;
	m_minionLayoutImage = nullptr;
	m_minionImage = nullptr;
}

void Minion::Initialize()
{	
	RefreshRenderables();
	UpdateSortLayer(g_defaultCardSortLayer);
}

void Minion::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	//if (m_isPositionLocked == false)
	//{
	//	if (m_isInputPriority)
	//	{
	//		Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetMouseClientPosition();
	//		mousePosition = Vector2(ClampFloat(mousePosition.x, 0.f, Window::GetInstance()->GetClientWidth()), ClampFloat(mousePosition.y, 0.f, Window::GetInstance()->GetClientHeight()));
	//		m_transform2D->SetLocalPosition(mousePosition);
	//	}		
	//}
	//else
	//{
	//	m_transform2D->SetLocalPosition(m_lockPosition);
	//}
	if(m_isPositionLocked)
		m_transform2D->SetLocalPosition(m_lockPosition);
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

	float minionHeight = g_minionPercentageOfClientWindowHeight * clientWindow->GetClientHeight();
	float minionWidth = minionHeight * g_minionAspectRatio;

	m_dimensionsInPixels = Vector2(minionWidth, minionHeight);
	Vector2 minionBottomRight = Vector2(Vector2::ZERO.x - (minionWidth * 0.5f), Vector2::ZERO.y - (minionHeight * 0.5f));

	Renderable2D* minionRenderable = new Renderable2D();

	//add minion base image
	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);

	Material* minionMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	minionMaterial->SetTexture(0, m_minionImage);

	minionRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), minionMaterial);
	minionMaterial = nullptr;

	//add overlay
	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	Material* minionLayoutMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	minionLayoutMaterial->SetTexture(0, m_minionLayoutImage);
	
	minionRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), minionLayoutMaterial);
	minionLayoutMaterial = nullptr;

	//add health and attack
	mb.CreateText2DInAABB2(minionBottomRight + (m_dimensionsInPixels * g_minionAttackCenterRatio), m_dimensionsInPixels * g_minionAttackDimensionsRatio, 1.f, Stringf("%i", m_attack), Rgba::WHITE); //attack
	mb.CreateText2DInAABB2(minionBottomRight + (m_dimensionsInPixels * g_minionHealthCenterRatio), m_dimensionsInPixels * g_minionHealthDimensionsRatio, 1.f, Stringf("%i", m_health), Rgba::WHITE); //health
	Material* minionText = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	minionText->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	minionRenderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), minionText);
	minionText = nullptr;

	m_renderables.push_back(minionRenderable);

	// add renderables to scene =========================================================================================
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	clientWindow = nullptr;
	theRenderer = nullptr;

	UpdateSortLayer(GetSortLayer());

}

Vector2 Minion::GetMinionDimensions()
{
	return m_dimensionsInPixels;
}

void Minion::OnLeftClicked()
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();

	//only allow left clicks on a minion if you are the controller
	if (playingState->m_activePlayer->m_playerId == m_controller)
	{
		if (m_isInputPriority == false)
		{
			bool hasTag = CheckForTag("charge");
			if ((m_age > 0 || hasTag) && m_hasAttackedThisTurn == false)
			{
				m_isInputPriority = true;
				m_isPositionLocked = false;
				UpdateSortLayer(g_sortLayerMax);

				TargetEffect* attackTarget = new TargetEffect(this);
				AddEffectToEffectQueue(attackTarget);

				//cleanup
				attackTarget = nullptr;
			}
		}
	}	
}

void Minion::OnRightClicked()
{
	TODO("Draw card renderable temporarily");
}
