//TODO: update for functions and etc if necessary. Empty cpp for now
#include "Game\Map\Tile.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Renderable2D.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\StringUtils.hpp"

Tile::Tile()
{
}

Tile::~Tile()
{
	delete(m_tags);
	m_tags = nullptr;

	m_tileDefinition = nullptr;
}

void Tile::Initialize()
{
	m_worldBounds = AABB2(Vector2((float)m_tileCoords.x * g_tileSize, (float)m_tileCoords.y * g_tileSize),
		Vector2(((float)m_tileCoords.x + 1) * g_tileSize, ((float)m_tileCoords.y + 1) * g_tileSize));

	//m_lockPosition = Vector2(m_tileCoords);
	//m_isPositionLocked = true;

	//float dimensions = Window::GetInstance()->GetClientWidth() * g_tilePercentageOfWindow;
	//AABB2 bounds = AABB2(Vector2::ZERO, Vector2::ONE * dimensions);

	////setup tile renderable
	//Renderable2D* renderable = new Renderable2D();

	//Renderer* theRenderer = Renderer::GetInstance();

	//MeshBuilder mb;
	//mb.FlushBuilder();

	//mb.CreateTexturedQuad2D(Vector2::ZERO, bounds.GetDimensions(), Vector2(m_tileDefinition->m_baseSpriteUVCoords.mins.x, m_tileDefinition->m_baseSpriteUVCoords.maxs.y), Vector2(m_tileDefinition->m_baseSpriteUVCoords.maxs.x, m_tileDefinition->m_baseSpriteUVCoords.mins.y), Rgba::WHITE);
	//Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	//materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"));

	//renderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), materialInstance);
	//m_renderables.push_back(renderable);

	//for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	//{
	//	m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	//}

	//m_lockPosition = Vector2(m_tileCoords) * dimensions;
	//m_transform2D->SetLocalPosition(m_lockPosition);
	//PreRender();

	//materialInstance = nullptr;
	//theRenderer = nullptr;
}

Vector2 Tile::GetWorldSpaceCoordinates()
{
	return m_worldBounds.mins;
}

AABB2 Tile::GetWorldSpaceBounds()
{
	return m_worldBounds;	
}

void Tile::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->DrawTexturedAABB(GetWorldSpaceBounds(), *theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), m_tileDefinition->m_baseSpriteUVCoords.mins, m_tileDefinition->m_baseSpriteUVCoords.maxs, m_tint);
	
	theRenderer->SetTexture(*theRenderer->CreateOrGetTexture("default"));

	if (g_showBlockedTileData)
	{
		int value = m_tileDefinition->m_allowsWalking == true ? 0 : 1;
		std::string doesBlock = Stringf("%i", value);
		theRenderer->DrawText2DCentered(GetWorldSpaceBounds().GetCenter(), doesBlock.c_str(), g_tileSize, Rgba::WHITE, 1.f, theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	}

	theRenderer = nullptr;
}
