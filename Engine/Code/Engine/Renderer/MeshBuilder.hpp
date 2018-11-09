#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include <vector>
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Vertex.hpp"
#include "Engine\Math\AABB2.hpp"
#include <functional>
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Utility\Graph.hpp"

class MeshBuilder
{
public:
	VertexBuilder m_stamp;
	std::vector<VertexBuilder> m_vertices;
	std::vector<int> m_indices;

	//additional information
	DrawPrimitiveType m_type;
	bool m_doesUseIndices;

public:
	void Begin(DrawPrimitiveType type, bool doesUseIndices);

	//utility methods
	void SetColor(const Rgba& color);
	void SetUV(float u, float v);
	void SetUV(const Vector2& uv);
	void SetNormal(const Vector3 & normal);
	void SetTangent(const Vector4& tangent);
	int PushVertex(Vector3 position);
	int PushIndex(int index);

	void AddTriIndices(int vertex0, int vertex1, int vertex2);
	void AddQuadIndices(int vertex0, int vertex1, int vertex2, int vertex3);
	int GetVertexCount();
	int GetIndexCount();
	VertexBuilder GetVertexAtIndex(int index);

	//cleanup
	void FlushBuilder();

	//load from file
	void LoadObjectFromFile(const std::string& objFilePath);
	Vector4 GenerateTangent(const Vector3& normal);

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

	

	// builder methods =========================================================================================

	//static methods
	void CreatePoint(const Vector3& center, const Rgba& tint, float scale);

	//3D
	void CreateCube(const Vector3& center, const Vector3& dimensions, const Rgba& tint);
	void CreateUVSphere(const Vector3& position, float radius, int wedges, int slices, const Rgba& tint);
	void CreateTexturedQuad3D(const Vector3 & center, const Vector2 & dimensions, const Vector2 & texCoordsAtMins, const Vector2 & texCoordsAtMaxs, const Rgba& tint);
	void CreateQuad3D(const Vector3& center, const Vector2& dimensions, const Rgba& tint);
	void CreateLine(const Vector3& positionStart, const Vector3& positionEnd, const Rgba& color);
	void CreateLine2D(const Vector2& positionStart, const Vector2& positionEnd, const Rgba& color);
	void CreateBasis(const Vector3& positionStart, float scale);
	void CreateBasis(const Matrix44& basis, const Vector3& position, float scale);
	void CreateStarQuads3D(const Vector3& center, const Vector3& dimensions, const Rgba& tint);
	void CreateTiledQuad3D(const Vector3& center, const Vector3& dimensions, const Vector2& cellDimensions, const Rgba & tint);
	void CreateBillboardQuad3d(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2& dimensions, const Rgba& tint);

	//2D
	void CreateQuad2D(const Vector2& center, const Vector2& dimensions, const Rgba& tint);
	void CreateLine2D(const Vector2& startPosition, const Vector2& endPosition, float width, const Rgba& tint);
	void CreateQuad2D(const AABB2& drawBounds, const Rgba& tint);
	void CreateTexturedQuad2D(const Vector2& center, const Vector2& dimensions, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint);
	void CreateTiledQuad2D(const Vector2& center, const Vector2& dimensions, const Vector2& cellDimensions, const Rgba & tint);
	void CreateScrollableQuad2D(const Vector2& center, const Vector2& dimensions, const Rgba& tint);

	//text
	void CreateText2D(const Vector2& center, float cellHeight, float aspectScale, const std::string& text, const Rgba& tint = Rgba::WHITE);
	void CreateText2DInAABB2(const Vector2& center, const Vector2& dimensions, float aspectScale, const std::string& text, const Rgba& tint = Rgba::WHITE);
	void CreateText2DFromPoint(const Vector2 & startPos, float cellHeight, float aspectScale, const std::string & text, const Rgba & tint);


	//surface patch implementation
	void CreateFromSurfacePatch(std::function<Vector3(float, float)> SurfacePatchFunc, const Vector2& uvRangeMin, const Vector2& uvRangeMax, const IntVector2& sampleFrequency, float cellUniformScale, const Rgba& tint);

	//templated functions
	template <class T>
	void CreateGraph(const AABB2& bounds, Graph<T>* graph, const Rgba& graphBackgroundColor = Rgba::BLACK, const Rgba& graphColor = Rgba::YELLOW, const Rgba& textColor = Rgba::WHITE)
	{
		CreateQuad2D(bounds.GetCenter(), bounds.GetDimensions(), graphBackgroundColor);

		int dataSize = (int)graph->m_dataPoints.size();
		float barWidth = (bounds.maxs.x - bounds.mins.x)/(float)dataSize;
		double maxValueInDataRange = graph->GetLargestDataPoint();

		for (int dataIndex = 0; dataIndex < dataSize; ++dataIndex)
		{
			float barHeight = RangeMapFloat((float)graph->m_dataPoints[dataIndex], 0.0f, 0.5f, bounds.mins.y, bounds.maxs.y);
			AABB2 dataBounds;
			Vector2 mins = Vector2(bounds.mins.x + (barWidth * (float)dataIndex), bounds.mins.y);
			Vector2 maxs = Vector2(bounds.mins.x + (barWidth * (float)(dataIndex + 1.f)), barHeight);

			dataBounds.mins = mins;
			dataBounds.maxs = maxs;

			CreateQuad2D(dataBounds.GetCenter(), dataBounds.GetDimensions(), graphColor);
		}		
	}
};

