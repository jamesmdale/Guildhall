#include "Game\Effects\DerivedEffects\TargetEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Minion.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\Actions\Action.hpp"
#include "Engine\core\StringUtils.hpp"


TargetEffect::TargetEffect(Character* attackingWidget)
{
	m_attackingWidget = attackingWidget;
	m_attackingWidget->UpdateSortLayer(g_sortLayerMax - 1);

	//create target widget
	m_targetWidget = new Widget();
	m_targetWidget->m_renderScene = attackingWidget->m_renderScene;
	m_targetWidget->UpdateSortLayer(g_sortLayerMax);
	RefreshTargetRenderable();
}

TargetEffect::~TargetEffect()
{
	delete(m_targetWidget);
	m_targetWidget = nullptr;
}

void TargetEffect::Update(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();
	m_targetWidget->m_transform2D->SetLocalPosition(mouseCoordinates);
	theInput = nullptr;

	m_targetWidget->PreRender();
	UpdateInput();	
}

void TargetEffect::UpdateInput()
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();
	
	//player cancelled attack
	if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		m_isComplete = true;
	}
	else if (theInput->WasKeyJustPressed(theInput->MOUSE_LEFT_CLICK))
	{
		PlayingState* playingState = (PlayingState*)g_currentState;	

		std::vector<Character*>* targetableWidgets = playingState->GetCharacterWidgets();
		Character* selectedWidget = playingState->GetSelectedCharacter(*targetableWidgets);

		if (selectedWidget != nullptr)
		{
			TODO("ADD PLAYER TARGETING SUPPORT");
			Minion* minion = (Minion*)selectedWidget;
			if (minion->m_controller != m_attackingWidget->m_controller)
			{
				TODO("HANDLE TAUNT CONDITIONS");

				std::map<std::string, std::string> parameters = { {"attackerId", Stringf("%i", m_attackingWidget->m_characterId)}, {"targetId", Stringf("%i", minion->m_characterId)} };
				AddActionToRefereeQueue("attack", parameters);

				m_isComplete = true;
			}
			minion = nullptr;
		}
		
		//cleanup
		targetableWidgets = nullptr;
		playingState = nullptr;
	}

	theInput = nullptr;
}

void TargetEffect::RefreshTargetRenderable()
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();

	MeshBuilder mb;

	//delete renderables from scene
	m_targetWidget->DeleteRenderables();

	m_targetWidget->m_transform2D->SetLocalPosition(mouseCoordinates);

	//create new renderable
	Renderable2D* targetRenderable = new Renderable2D();

	float targetingDimension = Window::GetInstance()->GetClientHeight() * g_targetingPercentageOfClientWindowHeight;
	Vector2 targetingDimensions = Vector2(targetingDimension, targetingDimension);

	AABB2 quadAABB = AABB2(Vector2::ZERO, targetingDimensions.x, targetingDimensions.y);

	mb.CreateQuad2D(quadAABB, Rgba::WHITE);
	Material* targetMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	targetMaterial->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Target.png"));

	targetRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), targetMaterial);
	m_targetWidget->m_renderables.push_back(targetRenderable);

	m_attackingWidget->m_renderScene->AddRenderable(targetRenderable);

	m_targetWidget->UpdateSortLayer(m_targetWidget->GetSortLayer());

	//cleanup
	targetMaterial = nullptr;
	targetRenderable = nullptr;
	theInput = nullptr;
	theRenderer = nullptr;
}