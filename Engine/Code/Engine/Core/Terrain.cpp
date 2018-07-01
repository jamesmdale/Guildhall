#include "Engine\Core\Terrain.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Renderer\Renderer.hpp"

Terrain::Terrain()
{
}

void Terrain::GenerateMeshFromHeightMap()
{
	Renderable* renderable = new Renderable();
	Renderer* theRenderer = Renderer::GetInstance();

	MeshBuilder mb;	

	//delete all renderables
	DeleteRenderables();	
	
	mb.CreateFromSurfacePatch([this](float u, float v){ return this->GetTerrainVertexPositionAtUV(u,v); }, m_uvBounds.mins, m_uvBounds.maxs, m_heightMap->GetDimensions(), m_cellScale, Rgba::GRAY);
	
	renderable->AddMesh(mb.CreateMesh<VertexLit>());

	renderable->SetMaterial(theRenderer->CreateOrGetMaterial("terrain"));
	//renderable->GetMaterial()->SetTexture(renderable->GetMaterial()->GetNumTextures(), theRenderer->CreateOrGetTexture("Data/Images/checkers.png"));
	AddRenderable(renderable);

	m_renderables.push_back(renderable);

	m_worldBounds = AABB3(Vector3(m_uvBounds.mins.x * m_cellScale, 0.f, m_uvBounds.mins.y * m_cellScale),
		Vector3(m_uvBounds.maxs.x * m_cellScale, m_heightScale, m_uvBounds.maxs.y * m_cellScale));

	//cleanup
	renderable = nullptr;
}

Terrain::~Terrain()
{
	delete(m_heightMap);
	m_heightMap = nullptr;

	DebuggerPrintf("Terrain got deleted");
}

// getters =============================================================================

Vector3 Terrain::GetTerrainVertexPositionAtUV(float u, float v)
{
	int texelIndexX = (int)RangeMapFloat(u, m_uvBounds.mins.x * m_cellScale, m_uvBounds.maxs.x * m_cellScale, 0.f, (float)m_heightMap->GetWidth() - 1.f);
	int texelIndexY = (int)RangeMapFloat(v, m_uvBounds.mins.y * m_cellScale, m_uvBounds.maxs.y * m_cellScale, 0.f, (float)m_heightMap->GetHeight() - 1.f);

	TODO("Could use linear height map to round off terrain.");
	//
	texelIndexX = ClampInt(texelIndexX, 0, m_heightMap->GetDimensions().x - 1);
	texelIndexY = ClampInt(texelIndexY, 0, m_heightMap->GetDimensions().y - 1);

	Rgba texel = m_heightMap->GetTexel(texelIndexX, texelIndexY);
	float texelVal0To1 = texel.GetRedAsFloat();

	float yHeight = RangeMapFloat(texelVal0To1, 0.f, 1.f, 0.f, m_heightScale);

	Vector3 vertexPosition = Vector3(u, yHeight, v);
	return vertexPosition;
}

Vector3 Terrain::GetWorldCoorindateAtPositionXZ(Vector2 positionXZ)
{
	//get position in world space.
	Vector3 terrainPosition = m_transform->GetWorldPosition();
	Vector2 terrainPositionXZ = Vector2(terrainPosition.x, terrainPosition.z);

	Vector2 terrainWorldMin = Vector2(terrainPositionXZ.x + (m_uvBounds.mins.x * m_cellScale), terrainPositionXZ.y + (m_uvBounds.mins.y * m_cellScale));
	Vector2 terrainWorldMax = Vector2(terrainPositionXZ.x + (m_uvBounds.maxs.x * m_cellScale), terrainPositionXZ.y + (m_uvBounds.maxs.y * m_cellScale));

	positionXZ.x = ClampFloat(positionXZ.x, terrainWorldMin.x, terrainWorldMax.x);
	positionXZ.y = ClampFloat(positionXZ.y, terrainWorldMin.y, terrainWorldMax.y);

	//get closest whole values
	float scaledU = floorf(positionXZ.x);
	float scaledV = floorf(positionXZ.y);

	//get the decimal part of the position (acts as percentage value that we will use for lerp late)
	float percentageXIntoCell = positionXZ.x - scaledU; 
	float percentageZIntoCell = positionXZ.y - scaledV;

	Vector2 closestUV = Vector2(scaledU, scaledV);

	Vector2 cellBottomLeftUV = closestUV;
	Vector2 cellBottomRightUV = closestUV + Vector2(1, 0);
	Vector2 cellTopRightUV = closestUV + Vector2(1, 1);
	Vector2 cellTopLeftUV = closestUV + Vector2(0, 1);

	Vector3 vertexPositionBottomLeft = GetTerrainVertexPositionAtUV(cellBottomLeftUV.x, cellBottomLeftUV.y);
	Vector3 vertexPositionBottomRight = GetTerrainVertexPositionAtUV(cellBottomRightUV.x, cellBottomRightUV.y);
	Vector3 vertexPositionTopRight = GetTerrainVertexPositionAtUV(cellTopRightUV.x, cellTopRightUV.y);
	Vector3 vertexPositionTopLeft = GetTerrainVertexPositionAtUV(cellTopLeftUV.x, cellTopLeftUV.y);

	float lerpBottom = Interpolate(vertexPositionBottomLeft.y, vertexPositionBottomRight.y, percentageXIntoCell);
	float lerpTop = Interpolate(vertexPositionTopLeft.y, vertexPositionTopRight.y, percentageXIntoCell);
	float height = Interpolate(lerpBottom, lerpTop, percentageZIntoCell);

	return Vector3(positionXZ.x, height, positionXZ.y);
}

float Terrain::GetHeightAtPositionXZ(Vector2 positionXZ)
{
	return GetWorldCoorindateAtPositionXZ(positionXZ).y;
}

Vector3 Terrain::GetNormalAtPositionXZ(const Vector2& positionXZ)
{
	Vector3 position = GetWorldCoorindateAtPositionXZ(positionXZ);

	Vector3 positionRight = GetWorldCoorindateAtPositionXZ(Vector2(positionXZ.x + 1.f, positionXZ.y));
	Vector3 positionUp = GetWorldCoorindateAtPositionXZ(Vector2(positionXZ.x, positionXZ.y + 1.f));

	Vector3 dv = positionRight - position;
	Vector3 du = positionUp - position;

	Vector3 normal = CrossProduct(du, dv);

	return normal;
}




