#include "Game\Entity\HeroPower.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Gamestates\PlayingState.hpp"
#include "Game\Effects\DerivedEffects\AbilityTargetEffect.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\Board.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Meshbuilder.hpp"


//  =========================================================================================
//HeroPower::HeroPower(eClass classType)
//{
//	//HeroDefinition* heroDefinition = HeroDefinition::GetDefinitionByName(classType);
//}

//  =========================================================================================
void HeroPower::OnLeftClicked()
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();

	//only allow left clicks on a minion if you are the controller
	if (playingState->m_activePlayer->m_playerId == m_controller)
	{
		if (m_isInputPriority == false && m_usedThisTurn == false)
		{
			m_isInputPriority = true;
			m_isPositionLocked = false;
			UpdateSortLayer(g_sortLayerMax);

			std::map<std::string, std::string> params = { {"action", "damage"}, {"targetId", ""}, {"amount", Stringf("%i", 2)} };

			AbilityTargetEffect* attackTarget = new AbilityTargetEffect(this);

			attackTarget->m_actions.push_back(&ActionData("hero_power", params));

			AddEffectToEffectQueue(attackTarget);

			//cleanup
			attackTarget = nullptr;
		}
	}
}

//  =========================================================================================
void HeroPower::OnRightClicked()
{
	TODO("Display info");
}

//  =========================================================================================
void HeroPower::Initialize()
{
	RefreshRenderables();
}

//  =========================================================================================
void HeroPower::RefreshRenderables()
{
	//remove renderables from scene
	DeleteRenderables();

	Renderer* theRenderer = Renderer::GetInstance();
	PlayingState* gameState = (PlayingState*)g_currentState;
	MeshBuilder mb;

	AABB2 portraitQuad;

	switch (m_controller)
	{
	case SELF_PLAYER_TYPE:
	{
		portraitQuad = gameState->m_gameBoard->m_playerHeroAbilityQuad;
		m_transform2D->SetLocalPosition(portraitQuad.GetCenter());
		break;
	}

	case ENEMY_PLAYER_TYPE:
	{
		portraitQuad = gameState->m_gameBoard->m_enemyHeroAbilityQuad;
		m_transform2D->SetLocalPosition(portraitQuad.GetCenter());
		break;
	}
	}

	m_dimensionsInPixels = portraitQuad.GetDimensions();
	Vector2 abilityBottomRight = Vector2(Vector2::ZERO.x - (m_dimensionsInPixels.x * 0.5f), Vector2::ZERO.y - (m_dimensionsInPixels.y * 0.5f));

	Renderable2D* heroRenderable = new Renderable2D();

	// create hero power background =========================================================================================
	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));

	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture(Stringf("Data/Images/Heroes/Powers/%s.jpg", m_name.c_str())));

	heroRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;

	// create hero power overlay renderable =========================================================================================

	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Template/Image_Hexgon_Frame.png"));

	heroRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;

	// create hero power attack/health =========================================================================================
	mb.CreateText2DInAABB2(abilityBottomRight + (m_dimensionsInPixels * g_heroPowerManaCostCenterRatio), m_dimensionsInPixels * g_heroPowerManaCostDimensionsRatio, 1.f, Stringf("%i", m_cost), Rgba::WHITE); //cost
	materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	heroRenderable->AddRenderableData(5, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;

	m_renderables.push_back(heroRenderable);

	// add renderables to scene =========================================================================================
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	UpdateSortLayer(3);
	UpdateRenderable2DFromTransform();

	heroRenderable = nullptr;
	gameState = nullptr;
	theRenderer = nullptr;
}
