#pragma once
#include <vector>
#include "Engine\Renderer\VertexLayout.hpp"
#include "Engine\Renderer\VertexBuffer.hpp"
#include "Engine\Renderer\IndexBuffer.hpp"
#include "Engine\Core\Vertex.hpp"
#include "Engine\Renderer\RendererTypes.hpp"

class MeshBuilder;

struct DrawInstruction
{
	DrawInstruction(){};

	DrawInstruction(int primitiveType, int startIndex, bool isUsingIndicies)
	{
		m_primitiveType = (DrawPrimitiveType)primitiveType;
		m_startIndex = startIndex;
		m_isUsingIndices = isUsingIndicies;
	}

	DrawPrimitiveType m_primitiveType; //don't need if you 
	int m_startIndex;
	bool m_isUsingIndices;	
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void SetIndices(int vertexStride, int count, const int* indices);
	void ClearIndices();

	//void SetVertices(int vertexStride, int count, const std::vector<VertexBuilder>& vertexBuilder); //currently unused.  Just another convenience method from builder

	void ClearVertices();
	void SetDrawInstructions(DrawPrimitiveType drawPrimitiveType, int startIndex, bool isUsingIndices);
	const int GetVertexStride() { return m_layout->m_stride;};

	//templates
	template <typename VERTEXTYPE>
	void SetVertices(int count, const VERTEXTYPE* vertices)
	{
		// this is a form of duck-typing.  Works for any class that has a 
		// LAYOUT defined that is a VertexLayout;
		m_layout = &VERTEXTYPE::s_layout;

		m_vbo->m_vertexStride = m_layout->GetStride();
		m_vbo->m_vertexCount = count;
		m_vbo->CopyToGPU(m_layout->GetStride() * count, vertices);		
	}

	//called using FromBuilder<VERTEXTYPE>(...)
	template <typename VERTEXTYPE>
	void FromBuilderForType(const MeshBuilder& meshBuilder)
	{
		int vertexCount = (int)meshBuilder.m_vertices.size(); 
		VERTEXTYPE* temp = (VERTEXTYPE*)malloc( sizeof(VERTEXTYPE) * vertexCount ); 

		for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) 
		{
			// copy each vertex
			temp[vertexIndex] = VERTEXTYPE( meshBuilder.m_vertices[vertexIndex] ); 
		}

		//set vertices and indices
		SetVertices<VERTEXTYPE>( vertexCount, temp );

		if(m_drawInstruction.m_isUsingIndices)
		{
			SetIndices(sizeof(int), (int)meshBuilder.m_indices.size(), meshBuilder.m_indices.data());
		}		

		// free our temp buffer
		free( temp );
	}

public:
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;;
	DrawInstruction m_drawInstruction;
    const VertexLayout* m_layout = nullptr; 
};

