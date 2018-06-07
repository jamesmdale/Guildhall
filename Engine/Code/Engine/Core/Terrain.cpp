#include "Engine\Core\Terrain.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\Rgba.hpp"

Terrain::Terrain()
{
}

void Terrain::GenerateMeshFromHeightMap()
{
	/*MeshBuilder mb;
	mb.CreateFromSurfacePatch(CreateMeshFromImage, m_bounds.mins, m_bounds.maxs, m_heightMap->GetDimensions(), Rgba::WHITE);
	m_renderable->SetMesh(mb.CreateMesh<VertexLit>());*/
}

Terrain::~Terrain()
{
	delete(m_heightMap);
	m_heightMap = nullptr;
}

Vector3 Terrain::CreateMeshFromImage(float u, float v)
{
	Rgba texel = m_heightMap->GetTexel(u, v);
	float texelIntensty = texel.GetRedAsFloat();

	float yHeight = RangeMapFloat(texelIntensty, 0.f, 1.f, 0.f, m_heightScale);

	Vector3 vertexPosition = Vector3(u, yHeight, v);
	return vertexPosition;
}
