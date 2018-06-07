#include "Engine\Core\Terrain.hpp"

Terrain::Terrain()
{
}


Terrain::~Terrain()
{
	delete(m_heightMap);
	m_heightMap = nullptr;
}

Vector3 Terrain::CreateMeshFromImage(float u, float v)
{
	return Vector3::ONE;
}
