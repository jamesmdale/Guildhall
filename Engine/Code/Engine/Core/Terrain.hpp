#include "Engine\Core\GameObject.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\Vector3.hpp"

class Terrain : public GameObject
{
public:
	Terrain();

	Vector3 CreateMeshFromImage(float u, float v);

	Terrain(const std::string& name, Vector3 position, AABB2 bounds, float heightScale, const char* filePath) : GameObject(name)
	{
		m_uvBounds = bounds;
		m_heightScale = heightScale;

		m_heightMap = new Image(filePath);
	}	

	void GenerateMeshFromHeightMap();

	//overrides
	virtual ~Terrain() override;

public:
	AABB2 m_uvBounds;
	float m_heightScale;
	Image* m_heightMap = nullptr;
};
