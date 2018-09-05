//TODO: update for functions and etc if necessary. Empty cpp for now
#include "Game\Map\Tile.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Renderable2D.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"

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

	
	//setup tile renderable
	Renderable2D* renderable = new Renderable2D();
	Renderer* theRenderer = Renderer::GetInstance();

	MeshBuilder mb;
	mb.FlushBuilder();

	mb.CreateQuad2D(GetWorldSpaceBounds(), Rgba::WHITE);
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Cards/card_back.png"));

	renderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), materialInstance);
	m_renderables.push_back(renderable);

	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	theRenderer = nullptr;
}

AABB2 Tile::GetTileBounds()
{	
	return AABB2(Vector2(0, 0), Vector2(1, 1));
}

AABB2 Tile::GetWorldSpaceBounds()
{
	float xCoordinate = (float)m_tileCoords.x;
	float yCoordinate = (float)m_tileCoords.y;
	return AABB2(Vector2((float)xCoordinate, (float)yCoordinate),Vector2((float)xCoordinate + 1.f, (float)yCoordinate + 1.f)); //tiles are 1 across
}

void Tile::Render()
{

}
//void Tile::Update()
//{
//
//}