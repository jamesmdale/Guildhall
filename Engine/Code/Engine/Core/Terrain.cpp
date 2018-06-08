#include "Engine\Core\Terrain.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\Rgba.hpp"

Terrain::Terrain()
{
}

void Terrain::GenerateMeshFromHeightMap()
{
	MeshBuilder mb;
	
	mb.CreateFromSurfacePatch([this](float u, float v){ return this->CreateMeshFromImage(u,v); }, m_uvBounds.mins, m_uvBounds.maxs, m_heightMap->GetDimensions(), m_cellScale, Rgba::GRAY);
	m_renderable->SetMesh(mb.CreateMesh<VertexLit>());
}

Terrain::~Terrain()
{
	delete(m_heightMap);
	m_heightMap = nullptr;
}

Vector3 Terrain::CreateMeshFromImage(float u, float v)
{
	float mappedU = (int)RangeMapFloat(u, m_uvBounds.mins.x, m_uvBounds.maxs.x, 0.f, m_heightMap->GetWidth() - 1);
	float mappedV = (int)RangeMapFloat(v, m_uvBounds.mins.y, m_uvBounds.maxs.y, 0.f, m_heightMap->GetHeight() - 1);

	TODO("Could use linear height map to round off terrain.");
	//
	Rgba texel = m_heightMap->GetTexel(mappedU, mappedV);
	float texelVal0To1 = texel.GetRedAsFloat();

	//need to add linear sampled
	float yHeight = RangeMapFloat(texelVal0To1, 0.f, 1.f, 0.f, m_heightScale);

	Vector3 vertexPosition = Vector3(u, yHeight, v);
	return vertexPosition;
}

float Terrain::GetHeightAtPosition(Vector2 position)
{
	return 0;
}


