#pragma once
#include "engine\math\Vector2.hpp"
#include "engine\core\Rgba.hpp"
#include "engine\math\Vector3.hpp"
#include <vector>

struct VertexAttribute;
class VertexLayout;

//builder = master incudes all data across all vertex types (lit, pcu, etc)
struct VertexBuilder
{	
	Vector3 position;	
	Rgba color;
	Vector2 uv;

	//lit
	Vector3 normal;
	Vector4 tangent;
};

struct VertexPCU
{
	VertexPCU(){};
	VertexPCU(const Vector3& position, const Rgba& color, const Vector2& uvs)
	{
		m_position = position;
		m_color = color;
		m_UVs = uvs;
	};

	VertexPCU(const VertexBuilder& other)
	{
		m_position = other.position;
		m_color = other.color;
		m_UVs = other.uv;
	};

	//represents a generic 2d/3d textured vertex for any draw object (quads, lines, etc)
	Vector3 m_position; //0-12
	Rgba m_color; //12-16
	Vector2 m_UVs; //16-24

	static const VertexAttribute s_attributes[];
	static const VertexLayout s_layout;
};

struct VertexLit
{
	VertexLit(){};
	VertexLit(const Vector3& position, const Rgba& color, const Vector2& uvs, const Vector3& normal, const Vector4& tangent)
	{
		m_position = position;
		m_color = color;	
		m_UVs = uvs;
		m_normal =  normal;
		m_tangent = tangent;
	};

	VertexLit(const VertexBuilder& other)
	{
		m_position = other.position;
		m_color = other.color;		
		m_UVs = other.uv;
		m_normal = other.normal;	
		m_tangent = other.tangent;
	};

	Vector3 m_position; 	
	Rgba m_color;
	Vector2 m_UVs; 
	Vector3 m_normal;
	Vector4 m_tangent;

	static const VertexAttribute s_attributes[];
	static const VertexLayout s_layout;
};


std::vector<VertexPCU> CopyPCUVerticesFromVertexBuilder(const std::vector<VertexBuilder>& builder);
std::vector<VertexLit> CopyLitVerticesFromVertexBuilder(const std::vector<VertexBuilder>& builder);