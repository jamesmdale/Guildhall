#include "Engine\Core\Terrain.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\Rgba.hpp"

Terrain::Terrain()
{
}

void Terrain::GenerateMeshFromHeightMap()
{
	MeshBuilder mb;
	
	mb.CreateFromSurfacePatch([this](float u, float v){ return this->GetTerrainVertexPositionAtUV(u,v); }, m_uvBounds.mins, m_uvBounds.maxs, m_heightMap->GetDimensions(), m_cellScale, Rgba::GRAY);
	m_renderable->SetMesh(mb.CreateMesh<VertexLit>());
}

Terrain::~Terrain()
{
	delete(m_heightMap);
	m_heightMap = nullptr;
}

Vector3 Terrain::GetTerrainVertexPositionAtUV(float u, float v)
{
	int texelIndexX = (int)RangeMapFloat(u, m_uvBounds.mins.x * m_cellScale, m_uvBounds.maxs.x * m_cellScale, 0.f, m_heightMap->GetWidth() - 1);
	int texelIndexY = (int)RangeMapFloat(v, m_uvBounds.mins.y * m_cellScale, m_uvBounds.maxs.y * m_cellScale, 0.f, m_heightMap->GetHeight() - 1);

	TODO("Could use linear height map to round off terrain.");
	//
	Rgba texel = m_heightMap->GetTexel(texelIndexX, texelIndexY);
	float texelVal0To1 = texel.GetRedAsFloat();

	float yHeight = RangeMapFloat(texelVal0To1, 0.f, 1.f, 0.f, m_heightScale);

	Vector3 vertexPosition = Vector3(u, yHeight, v);
	return vertexPosition;
}

float Terrain::GetHeightAtPositionXZ(Vector2 positionXZ)
{
	//get position in world space.
	Vector3 terrainPosition = m_transform->GetWorldPosition();
	Vector2 terrainPositionXZ = Vector2(terrainPosition.x, terrainPosition.z);
	
	Vector2 terrainWorldMin = Vector2(terrainPositionXZ.x + (m_uvBounds.mins.x * m_cellScale), terrainPositionXZ.y + (m_uvBounds.mins.y * m_cellScale));
	Vector2 terrainWorldMax = Vector2(terrainPositionXZ.x + (m_uvBounds.maxs.x * m_cellScale), terrainPositionXZ.y + (m_uvBounds.maxs.y * m_cellScale));

	//positionXZ = positionXZ + terrainWorldMin;
// 	positionXZ.x = RangeMapFloat(positionXZ.x, m_uvBounds.mins.x, m_uvBounds.maxs.x, terrainWorldMin.x, terrainWorldMax.x);
// 	positionXZ.y = RangeMapFloat(positionXZ.y, m_uvBounds.mins.y, m_uvBounds.maxs.y, terrainWorldMin.y, terrainWorldMax.y);

	//float x = RangeMapFloat(positionXZ.x, terrainWorldMin.x, terrainWorldMax.y, )

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
	float outHeight = Interpolate(lerpBottom, lerpTop, percentageZIntoCell);

	return outHeight;	
}


