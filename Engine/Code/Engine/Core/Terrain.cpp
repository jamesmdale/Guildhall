#include "Engine\Core\Terrain.hpp"

Terrain::Terrain()
{
}


Terrain::~Terrain()
{
	delete(m_heightMap);
	m_heightMap = nullptr;
}

void Terrain::CreateMeshFromImage(float u, float v)
{

}
