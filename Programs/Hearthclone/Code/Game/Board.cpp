#include "Game\Board.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"

//board tints
Rgba boardBaseColor = Rgba(228, 187, 118, 255); //light brown
Rgba boardHandColor = Rgba(161, 156, 154);	//gray

Rgba enemyPortraitColor = Rgba::RED;
Rgba playerPortraitColor = Rgba::BLUE;

Rgba heroAbilityColor = Rgba::ORANGE;
Rgba heroWeaponColor = Rgba(75, 75, 75, 255);

Board::Board()
{
	m_sortLayer = g_defaultBoardSortLayer;
}


Board::~Board()
{
}

void Board::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();	
	m_dimensionsInPixels = Window::GetInstance()->GetClientDimensions();

	//clear renderbale list if we've already initialized once.
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

	Renderable2D* boardRenderable = new Renderable2D();

	//create board layout
	CreateBoardMeshesForRenderable(boardRenderable);

	//add textured elements to board
	CreateBoardTexturedMeshesForRenderable(boardRenderable);

	//add text to board
	CreateBoardTextMeshesForRenderable(boardRenderable);

	//add renderable to scene
	m_renderables.push_back(boardRenderable);


	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	boardRenderable = nullptr;
	theRenderer = nullptr;
}


void Board::CreateBoardMeshesForRenderable(Renderable2D* renderable)
{
	Renderer* theRenderer = Renderer::GetInstance();

	MeshBuilder mb;
	mb.FlushBuilder();

	Window* clientWindow = Window::GetInstance();

	Vector2 clientWindowDimensions = clientWindow->GetClientDimensions();

	Vector2 handDimensions = Vector2(clientWindowDimensions.x, clientWindowDimensions.y * g_boardHandPercentageOfClientWindow);
	Vector2 battlefieldDimensions = Vector2(clientWindowDimensions.x, clientWindowDimensions.y * g_boardBattlefieldPercentageOfClientWindow);
	Vector2 heroPortraitDimensions = clientWindowDimensions * g_heroPortraitPercentageOfClientWindow;
	Vector2 heroWeaponDimensions = clientWindowDimensions * g_heroWeaponPercentageOfClientWindow;
	Vector2 heroAbilityDimensions = clientWindowDimensions * g_heroAbilityPercentageOfClientWindow;
	Vector2 manaDimensions = clientWindowDimensions * g_manaPercentageOfClientWindow;
	Vector2 endTurnDimensions = clientWindowDimensions * g_endTurnButtonPercentageOfClientWindow;

	mb.CreateQuad2D(AABB2(clientWindow->GetClientWindow().mins, clientWindow->GetClientWindow().maxs), boardBaseColor); //whole board

	Material* material = Material::Clone(theRenderer->CreateOrGetMaterial("default"));

	renderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("default")));
    
	// create quads for each hand =========================================================================================
	Vector2 enemyHandCenter = Vector2(clientWindowDimensions.x * 0.5f
		, clientWindowDimensions.y - (handDimensions.y * 0.5f));
	Vector2 playerHandCenter = Vector2(clientWindowDimensions.x * 0.5f
		, handDimensions.y * 0.5f);
	
	//store off hand quads
	m_enemyHandQuad = AABB2(enemyHandCenter, handDimensions.x * 0.5f, handDimensions.y * 0.5f);
	m_playerHandQuad = AABB2(playerHandCenter, handDimensions.x * 0.5f, handDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyHandCenter, handDimensions, boardHandColor);
	mb.CreateQuad2D(playerHandCenter, handDimensions, boardHandColor);

	// create quads for each side of the battlefield =========================================================================================
	
	Vector2 enemyBattlefieldCenter = Vector2(clientWindowDimensions.x * 0.5f
		, clientWindowDimensions.y - ((battlefieldDimensions.y * 0.5f) + handDimensions.y));
	Vector2 playerBattlefieldCenter = Vector2(clientWindowDimensions.x * 0.5f
		, (battlefieldDimensions.y * 0.5f) + handDimensions.y);

	//store off battlefield quads
	m_enemyBattlfieldQuad = AABB2(enemyBattlefieldCenter, battlefieldDimensions.x * 0.5f, battlefieldDimensions.y * 0.5f);
	m_playerBattlfieldQuad = AABB2(playerBattlefieldCenter, battlefieldDimensions.x * 0.5f, battlefieldDimensions.y * 0.5f);
	
	mb.CreateQuad2D(enemyBattlefieldCenter, battlefieldDimensions, boardBaseColor);
	mb.CreateQuad2D(playerBattlefieldCenter, battlefieldDimensions, boardBaseColor);

	// create quads for hero portrait =========================================================================================
	Vector2 enemyPortraitCenter = Vector2(clientWindowDimensions.x * 0.5f
		, clientWindowDimensions.y - ((heroPortraitDimensions.y * 0.5f) + handDimensions.y));
	Vector2 playerPortraitCenter = Vector2(clientWindowDimensions.x * 0.5f
		, (heroPortraitDimensions.y * 0.5f) + handDimensions.y);

	//store off hero portrait quads
	m_enemyHeroPortraitQuad = AABB2(enemyPortraitCenter, heroPortraitDimensions.x * 0.5f, heroPortraitDimensions.y * 0.5f);
	m_playerHeroPortraitQuad = AABB2(playerPortraitCenter, heroPortraitDimensions.x * 0.5f, heroPortraitDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyPortraitCenter, heroPortraitDimensions, enemyPortraitColor);
	mb.CreateQuad2D(playerPortraitCenter, heroPortraitDimensions, Rgba::ORANGE);

	// create quads for hero ability =========================================================================================
	Vector2 enemyAbilityCenter = Vector2((clientWindowDimensions.x * 0.5f) + (heroPortraitDimensions.x * 0.5f) + (heroAbilityDimensions.x * 0.5f)
		, clientWindowDimensions.y - ((heroAbilityDimensions.y * 0.5f) + handDimensions.y));
	Vector2 playerAbilityCenter = Vector2((clientWindowDimensions.x * 0.5f) + (heroPortraitDimensions.x * 0.5f) + (heroAbilityDimensions.x * 0.5f)
		, (heroAbilityDimensions.y * 0.5f) + handDimensions.y);

	//store off hero portrait quads
	m_enemyHeroAbilityQuad = AABB2(enemyAbilityCenter, heroAbilityDimensions.x * 0.5f, heroAbilityDimensions.y * 0.5f);
	m_playerHeroAbilityQuad = AABB2(playerAbilityCenter, heroAbilityDimensions.x * 0.5f, heroAbilityDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyAbilityCenter, heroAbilityDimensions, Rgba::PURPLE);
	mb.CreateQuad2D(playerAbilityCenter, heroAbilityDimensions, Rgba::PURPLE);

	// create quads for hero weapon =========================================================================================
	Vector2 enemyWeaponCenter = Vector2((clientWindowDimensions.x * 0.5f) - (heroPortraitDimensions.x * 0.5f) - (heroWeaponDimensions.x * 0.5f)
		, clientWindowDimensions.y - ((heroWeaponDimensions.y * 0.5f) + handDimensions.y));
	Vector2 playerWeaponCenter = Vector2((clientWindowDimensions.x * 0.5f) - (heroPortraitDimensions.x * 0.5f) - (heroWeaponDimensions.x * 0.5f)
		, (heroAbilityDimensions.y * 0.5f) + handDimensions.y);

	//store off hero portrait quads
	m_enemyHeroWeaponQuad = AABB2(enemyWeaponCenter, heroWeaponDimensions.x * 0.5f, heroWeaponDimensions.y * 0.5f);
	m_playerHeroWeaponQuad = AABB2(playerWeaponCenter, heroWeaponDimensions.x * 0.5f, heroWeaponDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyWeaponCenter, heroWeaponDimensions, Rgba::WHITE);
	mb.CreateQuad2D(playerWeaponCenter, heroWeaponDimensions, Rgba::WHITE);

	// create quad for battlefield divider =========================================================================================
	Vector2 enemyManaCenter = Vector2(clientWindowDimensions.x - (manaDimensions.x * 0.5f)
		, clientWindowDimensions.y - ((manaDimensions.y * 0.5f) + handDimensions.y));
	Vector2 playerManaCenter = Vector2(clientWindowDimensions.x  - (manaDimensions.x * 0.5f)
		, (manaDimensions.y * 0.5f) + handDimensions.y);

	//store off hero portrait quads
	m_enemyManaQuad = AABB2(enemyManaCenter, manaDimensions.x * 0.5f, manaDimensions.y * 0.5f);
	m_playerManaQuad = AABB2(playerManaCenter, manaDimensions.x * 0.5f, manaDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyManaCenter, manaDimensions, Rgba::BLUE);
	mb.CreateQuad2D(playerManaCenter, manaDimensions, Rgba::BLUE);

	// create quad for battlefield divider =========================================================================================
	mb.CreateQuad2D(clientWindow->GetCenterOfClientWindow(), Vector2(clientWindowDimensions.x, clientWindowDimensions.y * g_boardBattlefieldCenterLineThicknessHeight), Rgba::BLACK);
	
	// create quad for battlefield divider =========================================================================================
	Vector2 endTurnCenter = Vector2(clientWindowDimensions.x - (endTurnDimensions.x * 0.5f)
		, clientWindowDimensions.y * 0.5f);

	//store off hero portrait quads
	m_endTurnQuad = AABB2(endTurnCenter, endTurnDimensions.x * 0.5f, endTurnDimensions.y * 0.5f);

	mb.CreateQuad2D(endTurnCenter, endTurnDimensions, Rgba::GREEN);

	// add mesh and material to renderable =========================================================================================
	renderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("default")));


	// cleanup =========================================================================================
	clientWindow = nullptr;
	theRenderer = nullptr;
}

void Board::CreateBoardTexturedMeshesForRenderable(Renderable2D * renderable)
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* clientWindow = Window::GetInstance();
	MeshBuilder mb;

	// create quads for decks =========================================================================================
	float playerDeckQuadHeight = m_playerHandQuad.maxs.y - (m_playerHandQuad.maxs.y - m_playerHandQuad.mins.y) * 0.5f;
	float playerDeckQuadWidth = (m_playerHandQuad.maxs.x - m_playerHandQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

	float enemyDeckQuadHeight = m_enemyHandQuad.maxs.y - (m_enemyHandQuad.maxs.y - m_enemyHandQuad.mins.y) * 0.5f;
	float enemyDeckQuadWidth = (m_enemyHandQuad.maxs.x - m_enemyHandQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

	float cardHeight = g_cardPercentageOfClientWindowHeight * clientWindow->GetClientHeight();
	float cardWidth = cardHeight * g_cardAspectRatio;

	Vector2 playerDeckCenter = Vector2(playerDeckQuadWidth * (g_maxHandSize), playerDeckQuadHeight);
	Vector2 enemyDeckCenter = Vector2(enemyDeckQuadWidth * (g_maxHandSize), enemyDeckQuadHeight);

	m_playerDeckQuad = AABB2(playerDeckCenter, cardWidth * 0.5f, cardHeight * 0.5f);
	m_enemyDeckQuad = AABB2(enemyDeckCenter, cardWidth * 0.5f, cardHeight * 0.5f);
	mb.CreateQuad2D(m_playerDeckQuad, Rgba::WHITE);
	mb.CreateQuad2D(m_enemyDeckQuad, Rgba::WHITE);

	// add mesh and material to renderable =========================================================================================
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));
	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Cards/card_back.png"));

	renderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), materialInstance);
	
	// cleanup =========================================================================================
	materialInstance = nullptr;
	clientWindow = nullptr;
	theRenderer = nullptr;
}

void Board::CreateBoardTextMeshesForRenderable(Renderable2D * renderable)
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder mb;
	mb.FlushBuilder();

	Window* clientWindow = Window::GetInstance();

	m_endTurnQuad.GetCenter();

	mb.CreateText2DInAABB2(m_endTurnQuad.GetCenter(), m_endTurnQuad.GetDimensions(), 4.f / 3.f, "END TURN", Rgba::BLACK);

	// add mesh and material to renderable =========================================================================================
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::BLACK));

	renderable->AddRenderableData(3, mb.CreateMesh<VertexPCU>(), materialInstance);

	// cleanup =========================================================================================
	clientWindow = nullptr;
	theRenderer = nullptr;
}
