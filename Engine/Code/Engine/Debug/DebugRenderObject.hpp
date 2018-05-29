#pragma once
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Transform.hpp"
#include "Engine\Math\Vector2.hpp"
#include <vector>
#include <string>
#include "Engine\Camera\Camera.hpp"
#include "Engine\Renderer\Shader.hpp"

enum DebugRenderType
{
	POINT_RENDER_TYPE,
	LINE_SEGMENT_RENDER_TYPE,
	BASIS_RENDER_TYPE,
	SPHERE_RENDER_TYPE,
	WIRE_SPHERE_RENDER_TYPE,
	WIRE_CUBE_RENDER_TYPE,
	QUAD_RENDER_TYPE,
	CUBE_RENDER_TYPE,
	QUAD_2D_RENDER_TYPE,
	LINE_2D_RENDER_TYPE,
	CROSSHAIR_2D_RENDER_TYPE,
	TEXT_2D_RENDER_TYPE,
	BILLBOARD_QUAD_RENDER_TYPE,
	NUM_DEBUG_RENDER_TYPES	
};

class DebugRenderObject
{
public:
	DebugRenderObject();
	~DebugRenderObject();

	void Update(float deltaSeconds);
	void Render();

//include all options that they could possibly have for the draw points.  Then use the variables we need for the type when we create the mesh
public:
	DebugRenderType m_type;	
	Rgba m_startColor;
	Rgba m_endColor;
	int m_depthType;
	float m_totalTimeToLive;
	float m_remainingTimeToLive;
	Camera* m_camera = nullptr;

	bool m_isDrawFinished = false;	

	Mesh* m_mesh;
	Shader* m_shader = nullptr;
	Texture* m_texture = nullptr;
	Transform m_transform;
};

