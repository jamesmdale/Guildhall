#include "Engine\Core\GameObject.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Math\Vector3.hpp"

class Terrain : public GameObject
{
public:
	Terrain();

	Vector3 CreateMeshFromImage(float u, float v);

	Terrain(const std::string& name, Vector3 position, AABB2 bounds, float heightScale, const char* filePath) : GameObject(name)
	{
		m_bounds = bounds;
		m_heightScale = heightScale;

		m_heightMap = new Image(filePath);

		/*	MeshBuilder mb;
			mb.CreateFromSurfacePatch(CreateMeshFromImage, bounds.mins, bounds.maxs, m_heightMap->GetDimensions(), Rgba::WHITE);
			m_renderable->SetMesh(mb.CreateMesh());*/
	}	

	//overrides
	virtual ~Terrain() override;

public:
	AABB2 m_bounds;
	float m_heightScale;
	Image* m_heightMap = nullptr;
};
