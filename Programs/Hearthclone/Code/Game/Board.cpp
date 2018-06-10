#include "Game\Board.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"

//board tints
Rgba boardBase = Rgba(228, 187, 118, 255); //light brown
Rgba boardHand = Rgba(161, 156, 154);	//gray

Rgba enemyPortrait = Rgba::RED;
Rgba playerPortrait = Rgba::BLUE;

Rgba heroAbility = Rgba::ORANGE;
Rgba heroWeapon = Rgba(75, 75, 75, 255);

Board::Board()
{
}

void Board::Initialize()
{
	Renderable2D* renderable = new Renderable2D();

	//create board layout
	Renderer* theRenderer = Renderer::GetInstance();	

	CreateBoardMeshesForRenderable(renderable);
	renderable->SetMaterial(new Material());
	renderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	renderable->GetMaterial()->SetTexture(renderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	renderable->GetMaterial()->SetSampler(renderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);
	
	m_renderables.push_back(renderable);

	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	theRenderer = nullptr;

}

Board::~Board()
{
}

void Board::CreateBoardMeshesForRenderable(Renderable2D* renderable)
{
	if ((int)renderable->m_meshes.size() != 0)
	{
		for (int meshIndex = 0; meshIndex < (int)renderable->m_meshes.size(); ++meshIndex)
		{
			delete(renderable->m_meshes[meshIndex]);
			renderable->m_meshes[meshIndex] = nullptr;
		}

		renderable->m_meshes.clear();
	}

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

	mb.CreateQuad2D(AABB2(clientWindow->GetClientWindow().mins, clientWindow->GetClientWindow().maxs), boardBase); //whole board
	renderable->AddMesh(mb.CreateMesh<VertexPCU>());
    
	// create quads for each hand =========================================================================================
	Vector2 enemyHandCenter = Vector2(clientWindowDimensions.x * 0.5f
		, clientWindowDimensions.y - (handDimensions.y * 0.5));
	Vector2 playerHandCenter = Vector2(clientWindowDimensions.x * 0.5f
		, handDimensions.y * 0.5);
	
	//store off hand quads
	m_enemyHandQuad = AABB2(enemyHandCenter, handDimensions.x * 0.5f, handDimensions.y * 0.5f);
	m_playerHandQuad = AABB2(playerHandCenter, handDimensions.x * 0.5f, handDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyHandCenter, handDimensions, boardHand);
	mb.CreateQuad2D(playerHandCenter, handDimensions, boardHand);

	// create quads for each side of the battlefield =========================================================================================
	
	Vector2 enemyBattlefieldCenter = Vector2(clientWindowDimensions.x * 0.5f
		, clientWindowDimensions.y - ((battlefieldDimensions.y * 0.5) + handDimensions.y));
	Vector2 playerBattlefieldCenter = Vector2(clientWindowDimensions.x * 0.5f
		, (battlefieldDimensions.y * 0.5) + handDimensions.y);

	//store off battlefield quads
	m_enemyBattlfieldQuad = AABB2(enemyBattlefieldCenter, battlefieldDimensions.x * 0.5f, battlefieldDimensions.y * 0.5f);
	m_playerBattlfieldQuad = AABB2(playerBattlefieldCenter, battlefieldDimensions.x * 0.5f, battlefieldDimensions.y * 0.5f);
	
	mb.CreateQuad2D(enemyBattlefieldCenter, battlefieldDimensions, Rgba::RED);
	mb.CreateQuad2D(playerBattlefieldCenter, battlefieldDimensions, Rgba::RED);

	// create quads for hero portrait =========================================================================================
	Vector2 enemyPortraitCenter = Vector2(clientWindowDimensions.x * 0.5f
		, clientWindowDimensions.y - ((heroPortraitDimensions.y * 0.5) + handDimensions.y));
	Vector2 playerPortraitCenter = Vector2(clientWindowDimensions.x * 0.5f
		, (heroPortraitDimensions.y * 0.5) + handDimensions.y);

	//store off hero portrait quads
	m_enemyHeroPortraitQuad = AABB2(enemyPortraitCenter, heroPortraitDimensions.x * 0.5f, heroPortraitDimensions.y * 0.5f);
	m_playerHeroPortraitQuad = AABB2(playerPortraitCenter, heroPortraitDimensions.x * 0.5f, heroPortraitDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyPortraitCenter, heroPortraitDimensions, Rgba::ORANGE);
	mb.CreateQuad2D(playerPortraitCenter, heroPortraitDimensions, Rgba::ORANGE);

	// create quads for hero ability =========================================================================================
	Vector2 enemyAbilityCenter = Vector2((clientWindowDimensions.x * 0.5f) + (heroPortraitDimensions.x * 0.5f) + (heroAbilityDimensions.x * 0.5f)
		, clientWindowDimensions.y - ((heroAbilityDimensions.y * 0.5) + handDimensions.y));
	Vector2 playerAbilityCenter = Vector2((clientWindowDimensions.x * 0.5f) + (heroPortraitDimensions.x * 0.5f) + (heroAbilityDimensions.x * 0.5f)
		, (heroAbilityDimensions.y * 0.5) + handDimensions.y);

	//store off hero portrait quads
	m_enemyHeroAbilityQuad = AABB2(enemyAbilityCenter, heroAbilityDimensions.x * 0.5f, heroAbilityDimensions.y * 0.5f);
	m_playerHeroAbilityQuad = AABB2(playerAbilityCenter, heroAbilityDimensions.x * 0.5f, heroAbilityDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyAbilityCenter, heroAbilityDimensions, Rgba::PURPLE);
	mb.CreateQuad2D(playerAbilityCenter, heroAbilityDimensions, Rgba::PURPLE);

	// create quads for hero weapon =========================================================================================
	Vector2 enemyWeaponCenter = Vector2((clientWindowDimensions.x * 0.5f) - (heroPortraitDimensions.x * 0.5f) - (heroWeaponDimensions.x * 0.5f)
		, clientWindowDimensions.y - ((heroWeaponDimensions.y * 0.5) + handDimensions.y));
	Vector2 playerWeaponCenter = Vector2((clientWindowDimensions.x * 0.5f) - (heroPortraitDimensions.x * 0.5f) - (heroWeaponDimensions.x * 0.5f)
		, (heroAbilityDimensions.y * 0.5) + handDimensions.y);

	//store off hero portrait quads
	m_enemyHeroWeaponQuad = AABB2(enemyWeaponCenter, heroWeaponDimensions.x * 0.5f, heroWeaponDimensions.y * 0.5f);
	m_playerHeroWeaponQuad = AABB2(playerWeaponCenter, heroWeaponDimensions.x * 0.5f, heroWeaponDimensions.y * 0.5f);

	mb.CreateQuad2D(enemyWeaponCenter, heroWeaponDimensions, Rgba::WHITE);
	mb.CreateQuad2D(playerWeaponCenter, heroWeaponDimensions, Rgba::WHITE);

	// create quad for battlefield divider =========================================================================================
	Vector2 enemyManaCenter = Vector2(clientWindowDimensions.x - (manaDimensions.x * 0.5f)
		, clientWindowDimensions.y - ((manaDimensions.y * 0.5) + handDimensions.y));
	Vector2 playerManaCenter = Vector2(clientWindowDimensions.x  - (manaDimensions.x * 0.5f)
		, (manaDimensions.y * 0.5) + handDimensions.y);

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

	// create mesh =========================================================================================
	renderable->AddMesh(mb.CreateMesh<VertexPCU>());


	clientWindow = nullptr;
}
