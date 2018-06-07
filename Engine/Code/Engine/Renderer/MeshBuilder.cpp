#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\Vertex.hpp"
#include "Engine\File\ObjectFileLoader.hpp"
#include "Engine\File\File.hpp"
#include <string>
#include "Engine\Core\StringUtils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "Engine\Renderer\BitmapFont.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Core\EngineCommon.hpp"

void MeshBuilder::Begin(DrawPrimitiveType type, bool doesUseIndices)
{
	m_type = type;
	m_doesUseIndices = doesUseIndices;

	TODO("Could expand this later in case we are stacking meshes in the builder");
	/*	if (use_indices) {
	m_draw.start_index = m_indices.size(); 
	} else {
	m_draw.start_index = m_vertices.size(); 
	}*/
	/*void end()
	{
	uint end_idx;
	if (m_draw.using_indices) {
	end_idx = m_indices.size(); 
	} else {
	end_idx = m_vertices.size(); 
	}

	m_draw.elem_count = end_idx - m_draw.start_index; 
	}*/
}



void MeshBuilder::SetColor(const Rgba& color)
{
	m_stamp.color = color;
}

void MeshBuilder::SetUV(float u, float v)
{
	m_stamp.uv = Vector2(u, v);
}

void MeshBuilder::SetUV(const Vector2& uv)
{
	m_stamp.uv = uv;
}

void MeshBuilder::SetNormal(const Vector3& normal)
{
	m_stamp.normal = normal;
}

void MeshBuilder::SetTangent(const Vector4& tangent)
{
	m_stamp.tangent = tangent;
}

int MeshBuilder::PushVertex(Vector3 position)
{
	m_stamp.position = position;
	m_vertices.push_back(m_stamp);

	return (int)m_vertices.size() - 1;
}

int MeshBuilder::PushIndex(int index)
{
	m_indices.push_back(index);
	return (int)m_indices.size() - 1;
}

void MeshBuilder::FlushBuilder()
{
	m_doesUseIndices = false;
	m_indices.clear();
	m_vertices.clear();
}

void MeshBuilder::AddTriIndices(int vertex0, int vertex1, int vertex2) //adds single triangle into index array
{
	PushIndex(vertex0);
	PushIndex(vertex1);
	PushIndex(vertex2);
}

void MeshBuilder::AddQuadIndices(int vertex0, int vertex1, int vertex2, int vertex3) //adds two triangles to index array
{
	PushIndex(vertex0);
	PushIndex(vertex1);
	PushIndex(vertex2);
	PushIndex(vertex2);
	PushIndex(vertex3);
	PushIndex(vertex0);
}

int MeshBuilder::GetVertexCount()
{
	return (int)m_vertices.size();
}

int MeshBuilder::GetIndexCount()
{
	//if we aren't using indices. Don't return a size. (fail safe in case data is bad)
	if(m_doesUseIndices)
	{
		return (int)m_indices.size();
	}

	return 0;
}

VertexBuilder MeshBuilder::GetVertexAtIndex(int index)
{
	return m_vertices[index];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Creation methods
void MeshBuilder::CreatePoint(const Vector3& center, const Rgba& tint, float scale)
{
	Begin(LINES_DRAW_PRIMITIVE, false); 

	//y axis
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(center - (Vector3::UP * scale));

	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(center + (Vector3::UP * scale));

	//z axis
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(center - (Vector3::FORWARD * scale));

	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(center + (Vector3::FORWARD * scale));

	//x axis
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(center - (Vector3::RIGHT * scale));

	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(center + (Vector3::RIGHT * scale));

	
}


void MeshBuilder::CreateCube(const Vector3& center, const Vector3& dimensions, const Rgba& tint)
{
	float xVal = 0.f;
	float yVal = 0.f;
	float zVal = 0.f;

	Vector2 texCoordsAtMins = Vector2::ZERO;
	Vector2 texCoordsAtMaxs = Vector2::ONE;

	if(dimensions.x != 0.f)
		xVal = dimensions.x/2.f;

	if(dimensions.y != 0.f)
		yVal = dimensions.y/2.f;

	if(dimensions.z != 0.f)
		zVal = dimensions.z/2.f;

	Begin(TRIANGLES_DRAW_PRIMITIVE, true); //begin is does use index buffer

	//front face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z - zVal));	

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z - zVal));	

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal,  center.y + yVal, center.z - zVal));	

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y + yVal, center.z - zVal));		

	AddQuadIndices(0, 1, 2, 3);

	//right face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, -1.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z - zVal));	

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, -1.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, -1.f, 1.f));
	PushVertex(Vector3(center.x + xVal,  center.y + yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, -1.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y + yVal, center.z - zVal));	

	AddQuadIndices(4, 5, 6, 7);

	//back face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(0.f, 0.f, 1.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(0.f, 0.f, 1.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(0.f, 0.f, 1.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal,  center.y + yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(0.f, 0.f, 1.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y + yVal, center.z + zVal));
	

	AddQuadIndices(8, 9, 10, 11);

	//left face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(-1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, 1.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(-1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, 1.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(-1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, 1.f, 1.f));
	PushVertex(Vector3(center.x - xVal,  center.y + yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(-1.f, 0.f, 0.f));
	SetTangent(Vector4(0.f, 0.f, 1.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y + yVal, center.z + zVal));
	

	AddQuadIndices(12, 13, 14, 15);

	//top face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(0.f, 1.f, 0.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y + yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(0.f, 1.f, 0.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y + yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(0.f, 1.f, 0.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal,  center.y + yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(0.f, 1.f, 0.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y + yVal, center.z + zVal));

	AddQuadIndices(16, 17, 18, 19);

	//bottom face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(0.f, -1.f, 0.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(0.f, -1.f, 0.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(0.f, -1.f, 0.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal,  center.y - yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(0.f, -1.f, 0.f));
	SetTangent(Vector4(-1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z - zVal));
	

	AddQuadIndices(20, 21, 22, 23);

}


void MeshBuilder::CreateUVSphere(const Vector3& position, float radius, int wedges, int slices, const Rgba& tint)
{
	//(0,0) to (1,1)
	Begin(TRIANGLES_DRAW_PRIMITIVE, true); //begin is does use index buffer

	for(int sliceIndex = 0; sliceIndex <= slices; ++sliceIndex)
	{
		float v = (float)sliceIndex / ((float)slices - 1.f);

		float azimuth = RangeMapFloat(v, 0.f, 1.f, -90.f, 90.f);

		for(int wedgeIndex = 0; wedgeIndex <= wedges; ++wedgeIndex)
		{
			float u = (float)wedgeIndex / ((float)wedges);
			float rotation = ConvertRadiansToDegrees(2.0f * GetPi() * u); //or rangemap

			SetUV(Vector2(u, v));
			SetColor(tint);
			Vector3 vertexPos = position + SphericalToCartesian(radius, rotation, azimuth);

			Vector3 normal = vertexPos - position;
			normal.Normalize();

			//derivative of (tan = d/du Polar)
			float x = -1.f * CosDegrees(azimuth) * SinDegrees(rotation) * radius;
			float y = 0.f;
			float z = CosDegrees(azimuth) * CosDegrees(rotation) * radius;

			Vector4 tangent = Vector4(x, y, z, 1.f);
			tangent.Normalize();

			SetTangent(tangent);
			SetNormal(normal);
			PushVertex(vertexPos);
		}
	}

	//now that we have all the verts...we need to construct faces via indices
	//best to think of vertexes as a 2D array
	for(int sliceIndex = 0;  sliceIndex < slices; ++sliceIndex)
	{
		for(int wedgeIndex = 0; wedgeIndex < wedges; ++wedgeIndex)
		{
			int bl_idx = wedges * sliceIndex + wedgeIndex;
			int tl_idx = bl_idx + wedges;
			int br_idx = bl_idx + 1;
			int tr_idx = br_idx + wedges;

			AddQuadIndices(bl_idx, br_idx, tr_idx, tl_idx);
		}
	}
}

//normally z = 0
void MeshBuilder::CreateQuad3D(const Vector3& center, const Vector2& dimensions, const Rgba& tint)
{
	MeshBuilder mb;

	float xVal = 0.f;
	float yVal = 0.f;

	Vector2 texCoordsAtMins = Vector2::ZERO;
	Vector2 texCoordsAtMaxs = Vector2::ONE;

	if(dimensions.x != 0.f)
		xVal = dimensions.x/2.f;

	if(dimensions.y != 0.f)
		yVal = dimensions.y/2.f;

	Begin(TRIANGLES_DRAW_PRIMITIVE, true); //begin is does use index buffer

						   //front face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, 0.f));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, 0.f));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x + xVal,  center.y + yVal, 0.f));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center.x - xVal, center.y + yVal, 0.f));	
	

	AddQuadIndices(0, 1, 2, 3);
}

void MeshBuilder::CreateQuad2D(const Vector2& center, const Vector2& dimensions, const Rgba& tint)
{
	CreateQuad3D(Vector3(center.x, center.y, 0.f), dimensions, tint);
}

void MeshBuilder::CreateQuad2D(const AABB2& drawBounds, const Rgba& tint)
{
	Vector2 center = drawBounds.GetCenter();
	CreateQuad3D(Vector3(center.x, center.y, 0.f), drawBounds.GetDimensions(), tint);
}

void MeshBuilder::CreateStarQuads3D(const Vector3& center, const Vector3& dimensions, const Rgba& tint)
{
	float xVal = 0.f;
	float yVal = 0.f;
	float zVal = 0.f;

	Vector2 texCoordsAtMins = Vector2::ZERO;
	Vector2 texCoordsAtMaxs = Vector2::ONE;

	if(dimensions.x != 0.f)
		xVal = dimensions.x/2.f;

	if(dimensions.y != 0.f)
		yVal = dimensions.y/2.f;

	if(dimensions.z != 0.f)
		zVal = dimensions.z/2.f;

	Begin(TRIANGLES_DRAW_PRIMITIVE, true); //begin is does use index buffer

	//center face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(Vector3(center.x, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	PushVertex(Vector3(center.x, center.y - yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	PushVertex(Vector3(center.x,  center.y + yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	PushVertex(Vector3(center.x, center.y + yVal, center.z + zVal));	

	AddQuadIndices(0, 1, 2, 3);

	//left angle face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	PushVertex(Vector3(center.x - xVal, center.y - yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	PushVertex(Vector3(center.x + xVal,  center.y + yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	PushVertex(Vector3(center.x + xVal, center.y + yVal, center.z + zVal));	

	AddQuadIndices(4, 5, 6, 7);

	//right angle face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z + zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	PushVertex(Vector3(center.x + xVal, center.y - yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	PushVertex(Vector3(center.x - xVal,  center.y + yVal, center.z - zVal));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	PushVertex(Vector3(center.x - xVal, center.y + yVal, center.z + zVal));	

	AddQuadIndices(8, 9, 10, 11);
}

void MeshBuilder::CreateLine(const Vector3& positionStart, const Vector3& positionEnd, const Rgba& color)
{
	MeshBuilder mb;

	Begin(LINES_DRAW_PRIMITIVE, false); 

	SetColor(color);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart);

	SetColor(color);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionEnd);

	//begin is does use index buffer
	//2 vertexes
	//no indices
	//color
}

void MeshBuilder::CreateLine2D(const Vector2 & positionStart, const Vector2 & positionEnd, const Rgba & color)
{
	Begin(LINES_DRAW_PRIMITIVE, false); 

	SetColor(color);
	SetUV(Vector2(0.f,0.f));
	PushVertex(Vector3(positionStart.x, positionStart.y, 0.f));

	SetColor(color);
	SetUV(Vector2(0.f,0.f));
	PushVertex(Vector3(positionEnd.x, positionEnd.y, 0.f));

	//begin is does use index buffer
	//2 vertexes
	//no indices
	//color
}

void MeshBuilder::CreateBasis(const Vector3& positionStart, float scale)
{
	Begin(LINES_DRAW_PRIMITIVE, false); 

	//y axis
	SetColor(Rgba::GREEN);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart);

	SetColor(Rgba::GREEN);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart + (Vector3::UP * scale));

	//z axis
	SetColor(Rgba::BLUE);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart);

	SetColor(Rgba::BLUE);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart + (Vector3::FORWARD * scale));

	//x axis
	SetColor(Rgba::RED);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart);

	SetColor(Rgba::RED);
	SetUV(Vector2(0.f,0.f));
	PushVertex(positionStart + (Vector3::RIGHT * scale));
}

TODO("Add text to a single mesh for drawing");
void MeshBuilder::CreateText2D(const Vector2& center, float cellHeight, float aspectScale, const std::string& text)
{
	Begin(TRIANGLES_DRAW_PRIMITIVE, true); 

	BitmapFont* font = Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont");

	float cellWidth = cellHeight * (font->m_baseAspect * aspectScale);
	int textLength = (int)text.length();

	float offsetFromCenterX = (textLength * cellWidth) * .5f;
	float offsetFromCenterY = cellHeight * .5f;

	Vector2 drawMins = Vector2(center.x - offsetFromCenterX, center.y - offsetFromCenterY);

	for(int charIndex = 0; charIndex < textLength; charIndex++)
	{	
		int vertSize = (int)m_vertices.size();
		AABB2 uvs = font->GetUVsForGlyph(text.at(charIndex));
		Vector2 texCoordsAtMins = uvs.mins;
		Vector2 texCoordsAtMaxs = uvs.maxs;
		AABB2 bounds = AABB2(Vector2(drawMins.x + (cellWidth * charIndex), drawMins.y), Vector2(drawMins.x + (cellWidth * (charIndex + 1)), drawMins.y + cellHeight));
		
		SetColor(Rgba::WHITE);
		SetUV(Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
		PushVertex(Vector3(bounds.mins.x, bounds.mins.y, 0));

		SetColor(Rgba::WHITE);
		SetUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));
		PushVertex(Vector3(bounds.maxs.x, bounds.mins.y, 0));	
	
		SetColor(Rgba::WHITE);
		SetUV(Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
		PushVertex(Vector3(bounds.maxs.x, bounds.maxs.y, 0));

		SetColor(Rgba::WHITE);
		SetUV(Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
		PushVertex(Vector3(bounds.mins.x, bounds.maxs.y, 0));		

		AddQuadIndices(vertSize, vertSize + 1, vertSize + 2, vertSize + 3);
	}
}

void MeshBuilder::CreateBillboardQuad3d(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2& dimensions, const Rgba& tint)
{
	MeshBuilder mb;

	float xVal = 0.f;
	float yVal = 0.f;

	int vertSize = (int)m_vertices.size();

	Vector2 texCoordsAtMins = Vector2::ZERO;
	Vector2 texCoordsAtMaxs = Vector2::ONE;

	if(dimensions.x != 0.f)
		xVal = dimensions.x/2.f;

	if(dimensions.y != 0.f)
		yVal = dimensions.y/2.f;

	Vector3 adjustedRight = right.GetNormalized() * xVal;
	Vector3 adjustedUp = up.GetNormalized() * yVal;

	Begin(TRIANGLES_DRAW_PRIMITIVE, true); //begin is does use index buffer

	//front face
	SetColor(tint);
	SetUV(Vector2(0.f,0.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center - adjustedRight - adjustedUp));

	SetColor(tint);
	SetUV(Vector2(1.f,0.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center + adjustedRight - adjustedUp));

	SetColor(tint);
	SetUV(Vector2(1.f,1.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center + adjustedRight + adjustedUp));

	SetColor(tint);
	SetUV(Vector2(0.f,1.f));
	SetNormal(Vector3(0.f, 0.f, -1.f));
	SetTangent(Vector4(1.f, 0.f, 0.f, 1.f));
	PushVertex(Vector3(center - adjustedRight + adjustedUp));	

	AddQuadIndices(vertSize, vertSize + 1, vertSize + 2, vertSize + 3);
}


void MeshBuilder::CreateFromSurfacePatch(std::function<Vector3(float, float)> SurfacePatchFunc, const Vector2& uvRangeMin, const Vector2& uvRangeMax, const IntVector2& sampleFrequency, const Rgba& tint)
{
	Begin(TRIANGLES_DRAW_PRIMITIVE, true); 

	float stepAmountX = (uvRangeMax.x - uvRangeMin.x)/sampleFrequency.x;
	float stepAmountY = (uvRangeMax.y - uvRangeMin.y)/sampleFrequency.y;

	for (float v = uvRangeMin.x; v < uvRangeMax.x; v += stepAmountX)
	{
		for (float u = uvRangeMin.y; u < uvRangeMax.y; u += stepAmountY)
		{
			Vector3  vertexPos = SurfacePatchFunc(u, v);

			Vector3 stepTowardNextU = SurfacePatchFunc(u + stepAmountX, v);
			Vector3 stepTowardNextV = SurfacePatchFunc(u, v + stepAmountY);
			Vector3 directionTowardU = vertexPos - stepTowardNextU;
			Vector3 directionTowardV = vertexPos - stepTowardNextV;

			Vector3 normal = CrossProduct(directionTowardU, directionTowardV);

			SetUV(Vector2(u, v));
			SetColor(tint);
			SetNormal(normal.GetNormalized());
			SetTangent(Vector4(directionTowardV.GetNormalized(), 1));

			PushVertex(vertexPos);	
		}
	}

	//now that we have all the verts...we need to construct faces via indices
	//best to think of vertexes as a 2D array
	for (int v = 0; v < sampleFrequency.y - 1; v++)
	{
		for (int u = 0; u < sampleFrequency.x - 1; u++)
		{
			int bottomLeftIndex = (v * sampleFrequency.x) + u;
			int topLeftIndex = bottomLeftIndex + sampleFrequency.x;
			int bottomRightIndex = bottomLeftIndex + 1;
			int topRightIndex = topLeftIndex + 1;

			AddQuadIndices(bottomLeftIndex, bottomRightIndex, topRightIndex, topLeftIndex);
		}
	}

	return;
}


void MeshBuilder::LoadObjectFromFile(const std::string& objFilePath)
{
	Begin(TRIANGLES_DRAW_PRIMITIVE, true); 

	std::vector<Vector3> vertices;
	std::vector<Vector2> uvs;
	std::vector<Vector3> normals;

	FILE* file = nullptr;
	errno_t errorVal = fopen_s( &file, objFilePath.c_str(), "r" );
	UNUSED(errorVal); //used for debugging in case we get a fileread error here

	std::string buffer = (const char*) FileReadToNewBuffer(objFilePath.c_str());

	std::vector<std::string> lines = SplitStringOnCharacter(buffer, '\n');
	RemoveStringsStartingWithString(lines, "#");
	RemoveEmptyStrings(lines);

	for(int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++)
	{
		std::string identifierType = SplitStringOnFirstWord(lines[lineIndex]);

		if(identifierType == "v")
		{			
			Vector3 vertex;
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "v");
			RemoveEmptyStrings(tokens);

			vertex.x = -1 * ConvertStringToFloat(tokens[0]);
			vertex.y = ConvertStringToFloat(tokens[1]);
			vertex.z = ConvertStringToFloat(tokens[2]);

			vertices.push_back(vertex);			
		}

		else if(identifierType == "vt")
		{			
			Vector2 uv;
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "vt");
			RemoveEmptyStrings(tokens);

			uv.x = ConvertStringToFloat(tokens[0]);
			uv.y = ConvertStringToFloat(tokens[1]);			

			uvs.push_back(uv);			
		}

		else if(identifierType == "vn")
		{			
			Vector3 normal;
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "vn");
			RemoveEmptyStrings(tokens);

			normal.x = -1 * ConvertStringToFloat(tokens[0]);
			normal.y = ConvertStringToFloat(tokens[1]);
			normal.z = ConvertStringToFloat(tokens[2]);

			normals.push_back(normal);		
		}

		//actuall build the meshbuilder
		else if(identifierType == "f")
		{			
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "f");
			RemoveEmptyStrings(tokens);
			int currentVertIndex = (int)m_vertices.size();

			for(int tokenIndex = 0; tokenIndex < (int)tokens.size(); tokenIndex++)
			{				
				std::vector<std::string> subTokens = SplitStringOnCharacter(tokens[tokenIndex], '/');

				int positionIndex = ConvertStringToInt(subTokens[0]);
				int uvIndex = ConvertStringToInt(subTokens[1]);
				int normalIndex = ConvertStringToInt(subTokens[2]);

				SetColor(Rgba::WHITE);				
				SetUV(uvs[uvIndex - 1]);
				SetNormal(normals[normalIndex - 1]);
				SetTangent(GenerateTangent(normals[normalIndex - 1]));
				PushVertex(vertices[positionIndex - 1]);
			}

			if((int)tokens.size() == 3)
			{
				AddTriIndices(currentVertIndex, currentVertIndex + 1, currentVertIndex + 2);
			}

			if((int)tokens.size() == 4)
			{
				AddQuadIndices(currentVertIndex, currentVertIndex + 1, currentVertIndex + 2, currentVertIndex + 3);
			}
		}
	}
	//position, uv, normal.....start pushing at end of m_vertices array
	return;
}

Vector4 MeshBuilder::GenerateTangent(const Vector3 & normal)
{
	Vector3 tangent = CrossProduct(normal, Vector3::UP);

	if(tangent.CompareZero())
	{
		tangent = CrossProduct(normal, Vector3::RIGHT);
	}

	tangent.Normalize();

	return Vector4(tangent, 1);
}
