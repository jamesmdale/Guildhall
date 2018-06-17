#include "Engine\Core\GameObject.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\AABB3.hpp"

class Terrain : public GameObject
{
public:
	Terrain();
	virtual ~Terrain() override;

	Terrain(const std::string& name, Vector3 position, AABB2 bounds, float cellScale, float heightScale, const char* filePath) : GameObject(name)
	{
		m_uvBounds = bounds;
		m_heightScale = heightScale;
		m_cellScale = cellScale;

		m_heightMap = new Image(filePath);
	}	

	void GenerateMeshFromHeightMap();

	// getters =============================================================================
	Vector3 GetTerrainVertexPositionAtUV(float u, float v);
	Vector3 GetWorldCoorindateAtPositionXZ(Vector2 positionXZ);
	float GetHeightAtPositionXZ(Vector2 positionXZ);
	Vector3 GetNormalAtPositionXZ(const Vector2& positionXZ);		

public:
	AABB2 m_uvBounds;
	float m_heightScale;
	float m_cellScale;
	Image* m_heightMap = nullptr;

	AABB3 m_worldBounds;
};


