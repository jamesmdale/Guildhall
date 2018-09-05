//TODO: update for functions and etc if necessary. Empty cpp for now
#include "Game\Map\Tile.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Renderable2D.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Window\Window.hpp"

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
	m_lockPosition = Vector2(m_tileCoords);
	m_isPositionLocked = true;

	float dimensions = Window::GetInstance()->GetClientWidth() * g_tilePercentageOfWindow;
	AABB2 bounds = AABB2(Vector2::ZERO, Vector2::ONE * dimensions);

	//setup tile renderable
	Renderable2D* renderable = new Renderable2D();

	Renderer* theRenderer = Renderer::GetInstance();

	MeshBuilder mb;
	mb.FlushBuilder();

	mb.CreateTexturedQuad2D(Vector2::ZERO, bounds.GetDimensions(), m_tileDefinition->m_baseSpriteUVCoords.mins, m_tileDefinition->m_baseSpriteUVCoords.maxs, Rgba::WHITE);
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"));

	renderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), materialInstance);
	m_renderables.push_back(renderable);

	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	m_lockPosition = Vector2(m_tileCoords) * dimensions;
	m_transform2D->SetLocalPosition(m_lockPosition);
	PreRender();

	materialInstance = nullptr;
	theRenderer = nullptr;
}

AABB2 Tile::GetTileBounds()
{	
	float dimensions = Window::GetInstance()->GetClientWidth() * g_tilePercentageOfWindow;
	return AABB2(Vector2::ZERO, Vector2::ONE * dimensions);
}

AABB2 Tile::GetWorldSpaceBounds()
{
	float clientWidth = Window::GetInstance()->GetClientWidth();

	float xCoordinate = (float)m_tileCoords.x;
	float yCoordinate = (float)m_tileCoords.y;
	return AABB2(Vector2((float)xCoordinate * (clientWidth *  g_tilePercentageOfWindow), (float)yCoordinate * (clientWidth *  g_tilePercentageOfWindow)),Vector2((float)xCoordinate + 1.f * (clientWidth *  g_tilePercentageOfWindow), (float)yCoordinate + 1.f * (clientWidth *  g_tilePercentageOfWindow))); //tiles are 1 across
}

void Tile::Render()
{

}
//void Tile::Update()
//{
//
//}