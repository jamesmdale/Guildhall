#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include <vector>
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Vertex.hpp"
#include "Engine\Math\AABB2.hpp"
#include <functional>

class MeshBuilder
{
public:
	void Begin(DrawPrimitiveType type, bool doesUseIndices);
	void SetColor(const Rgba& color);
	void SetUV(float u, float v);
	void SetUV(const Vector2& uv);
	void SetNormal(const Vector3 & normal);
	void SetTangent(const Vector4& tangent);
	int PushVertex(Vector3 position);
	int PushIndex(int index);

	template <typename VERTEXTYPE>
	Mesh* CreateMesh()
	{
		Mesh* mesh = new Mesh();
		mesh->m_drawInstruction = DrawInstruction(m_type, 0, m_doesUseIndices);
		TODO("Might want to evaluate element count in drawinstruction");

		mesh->FromBuilderForType<VERTEXTYPE>(*this);

		FlushBuilder();
		return mesh;
	}

	void FlushBuilder();

	void AddTriIndices(int vertex0, int vertex1, int vertex2);
	void AddQuadIndices(int vertex0, int vertex1, int vertex2, int vertex3);
	int GetVertexCount();
	int GetIndexCount();
	VertexBuilder GetVertexAtIndex(int index);

	//static methods
	void CreatePoint(const Vector3& center, const Rgba& tint, float scale);
	void CreateCube(const Vector3& center, const Vector3& dimensions, const Rgba& tint);
	void CreateUVSphere(const Vector3& position, float radius, int wedges, int slices, const Rgba& tint);
	void CreateQuad3D(const Vector3& center, const Vector2& dimensions, const Rgba& tint);
	void CreateLine(const Vector3& positionStart, const Vector3& positionEnd, const Rgba& color);
	void CreateLine2D(const Vector2& positionStart, const Vector2& positionEnd, const Rgba& color);
	void CreateBasis(const Vector3& positionStart, float scale);
	void CreateQuad2D(const Vector2& center, const Vector2& dimensions, const Rgba& tint);
	void CreateQuad2D(const AABB2& drawBounds, const Rgba& tint);
	void CreateStarQuads3D(const Vector3& center, const Vector3& dimensions, const Rgba& tint);
	void CreateText2D(const Vector2& center, float cellHeight, float aspectScale, const std::string& text);
	void CreateBillboardQuad3d(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2& dimensions, const Rgba& tint);

	//surface patch implementation
	void CreateFromSurfacePatch(std::function<Vector3(float, float)> SurfacePatchFunc, const Vector2& uvRangeMin, const Vector2& uvRangeMax, const IntVector2& sampleFrequency, float cellUniformScale, const Rgba& tint);

	//load from file
	void LoadObjectFromFile( const std::string& objFilePath);
	Vector4 GenerateTangent(const Vector3& normal);

public:
	VertexBuilder m_stamp;
	std::vector<VertexBuilder> m_vertices;
	std::vector<int> m_indices;

	//additional information
	DrawPrimitiveType m_type;
	bool m_doesUseIndices;	
};

