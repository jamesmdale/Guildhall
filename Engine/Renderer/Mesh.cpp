#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Renderer\RenderBuffer.hpp"
#include "Engine\Renderer\VertexBuffer.hpp"
#include "Engine\Renderer\IndexBuffer.hpp"
#include "Engine\Core\Vertex.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Renderer\Renderer.hpp"

Mesh::Mesh()
{
	m_ibo = new IndexBuffer();
	m_vbo = new VertexBuffer();
}


Mesh::~Mesh()
{
	delete(m_vbo);
	m_vbo = nullptr;

	delete(m_ibo);
	m_ibo = nullptr;
}

void Mesh::SetIndices(int vertexStride, int count, const int* indices)
{	
	m_ibo->m_indexStride = vertexStride;
	m_ibo->m_indexCount = count;
	m_ibo->CopyToGPU(vertexStride * count, indices);
}

void Mesh::ClearIndices()
{
	m_ibo->m_indexCount = -1;
	m_ibo->m_indexStride = -1;
}

TODO("Potentially another way to get the vertexes. Add later as necessary.");
//void Mesh::SetVertices(int vertexStride, int count, const std::vector<VertexBuilder>& vertexBuilder)
//{
//	std::vector<Vertex_3DPCU> vertices = CopyFromVertexBuilder(vertexBuilder);
//
//	m_vbo->m_vertexStride = vertexStride;
//	m_vbo->m_vertexCount = count;
//	m_vbo->CopyToGPU(vertexStride * count, vertices.data());
//}

void Mesh::ClearVertices()
{
	m_vbo->m_vertexCount = -1;
	m_vbo->m_vertexStride = -1;
	m_layout = nullptr;
}

void Mesh::SetDrawInstructions(DrawPrimitiveType drawPrimitiveType, int startIndex, bool isUsingIndices)
{
	m_drawInstruction.m_primitiveType = drawPrimitiveType;
	m_drawInstruction.m_startIndex = startIndex;
	m_drawInstruction.m_isUsingIndices = isUsingIndices;
}