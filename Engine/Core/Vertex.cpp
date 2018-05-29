#include "Engine\Core\Vertex.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\VertexLayout.hpp"

//pcu --------------------------------------------------------------------------------------------------------------------------------------------
const VertexAttribute VertexPCU::s_attributes[] = {
	VertexAttribute( "POSITION", FLOAT_RENDER_TYPE,         3, false, offsetof(VertexPCU, m_position) ),
	VertexAttribute( "COLOR",    UNSIGNED_BYTE_RENDER_TYPE, 4, true,  offsetof(VertexPCU, m_color) ),
	VertexAttribute( "UV",       FLOAT_RENDER_TYPE,         2, false, offsetof(VertexPCU, m_UVs) ),
	VertexAttribute(), // like a null terminator, how do we know the list is done;
}; 

const VertexLayout VertexPCU::s_layout = VertexLayout( (int)sizeof(VertexPCU), VertexPCU::s_attributes );

//lit --------------------------------------------------------------------------------------------------------------------------------------------
const VertexAttribute VertexLit::s_attributes[] = {
	VertexAttribute( "POSITION", FLOAT_RENDER_TYPE,         3, false, offsetof(VertexLit, m_position) ),	
	VertexAttribute( "COLOR",    UNSIGNED_BYTE_RENDER_TYPE, 4, true,  offsetof(VertexLit, m_color) ),
	VertexAttribute( "UV",       FLOAT_RENDER_TYPE,         2, false, offsetof(VertexLit, m_UVs) ),
	VertexAttribute( "NORMAL",   FLOAT_RENDER_TYPE,			3, false, offsetof(VertexLit, m_normal) ),
	VertexAttribute( "TANGENT", FLOAT_RENDER_TYPE,			4, false, offsetof(VertexLit, m_tangent)),
	VertexAttribute(), // like a null terminator, how do we know the list is done;
}; 

const VertexLayout VertexLit::s_layout = VertexLayout( (int)sizeof(VertexLit), VertexLit::s_attributes );


//copy functions --------------------------------------------------------------------------------------------------------------------------------------------
std::vector<VertexPCU> CopyPCUVerticesFromVertexBuilder(const std::vector<VertexBuilder>& builder)
{
	std::vector<VertexPCU> vertexes;
	for(VertexBuilder builderIterator : builder)
	{
		VertexPCU vertex = VertexPCU(builderIterator.position, builderIterator.color, builderIterator.uv);
		vertexes.push_back(vertex);
	}

	return vertexes;
}

std::vector<VertexLit> CopyLitVerticesFromVertexBuilder(const std::vector<VertexBuilder>& builder)
{
	std::vector<VertexLit> vertexes;
	for(VertexBuilder builderIterator : builder)
	{
		VertexLit vertex = VertexLit(builderIterator.position, builderIterator.color, builderIterator.uv, builderIterator.normal, builderIterator.tangent);
		vertexes.push_back(vertex);
	}

	return vertexes;
}

