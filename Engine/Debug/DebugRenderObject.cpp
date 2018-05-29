#include "Engine\Debug\DebugRenderObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Renderer\ShaderProgram.hpp"
#include "Engine\Renderer\BitmapFont.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Core\Rgba.hpp"

DebugRenderObject::DebugRenderObject()
{
	DebugRender* theDebugRenderer = DebugRender::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	m_shader = theDebugRenderer->GetDebugShader();
	m_texture = theRenderer->CreateOrGetTexture("default");

	theRenderer = nullptr;
	theDebugRenderer = nullptr;
}

DebugRenderObject::~DebugRenderObject()
{
	m_camera = nullptr;
	m_shader = nullptr;
	m_texture = nullptr;

	delete(m_mesh);
	m_mesh = nullptr;
}

void DebugRenderObject::Update(float deltaSeconds)
{
	//remaining time is infinite if already below zero
	if(m_remainingTimeToLive < 0)
	{
		return;
	}

	m_remainingTimeToLive -= deltaSeconds;

	if(m_remainingTimeToLive < 0)
	{
		m_isDrawFinished = true;
	}
}

void DebugRenderObject::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	
	theRenderer->SetTexture(*m_texture);

	float percentComplete = (m_totalTimeToLive - m_remainingTimeToLive) / m_totalTimeToLive;
	Rgba lerpColor = Interpolate(m_startColor, m_endColor, percentComplete);
	
	if(m_camera == nullptr)
	{
		theRenderer->SetCamera(theRenderer->m_defaultCamera);
	}
	else
	{
		theRenderer->SetCamera(m_camera);
	}

	switch(m_type)
	{
		case POINT_RENDER_TYPE:
		{
			break;
		}
		case LINE_SEGMENT_RENDER_TYPE:
		{
			break;
		}
		case BASIS_RENDER_TYPE:
		{
			break;
		}
		case SPHERE_RENDER_TYPE:
		{
			break;
		}
		case WIRE_SPHERE_RENDER_TYPE:
		{
			m_shader->SetFillMode(LINE_DRAW_MODE);
			break;
		}
		case WIRE_CUBE_RENDER_TYPE:
		{
			m_shader->SetFillMode(LINE_DRAW_MODE);
			break;
		}
		case QUAD_RENDER_TYPE:
		{
			m_shader->SetCullMode(CULL_MODE_NONE);
			break;
		}
		case CUBE_RENDER_TYPE:
		{
			break;
		}
		case QUAD_2D_RENDER_TYPE:
		{
			break;
		}

		case LINE_2D_RENDER_TYPE:
		{
			break;
		}

		case CROSSHAIR_2D_RENDER_TYPE:
		{
			break;
		}

		case TEXT_2D_RENDER_TYPE:
		{
			BitmapFont* font = Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont");
			Texture fontTexture = *font->m_spriteSheet.GetSpriteSheetTexture();
			theRenderer->SetTexture(fontTexture);
			/*m_shader->EnableColorBlending(BLEND_OP_ADD, BLEND_SOURCE_ALPHA, BLEND_ONE_MINUS_SOURCE_ALPHA);
			m_shader->EnableAlphaBlending(BLEND_OP_ADD, BLEND_ONE, BLEND_ONE);*/
			//m_shader->SetCullMode(CULL_MODE_NONE);
			break;
		}
	}	

	//special cases before draw
	m_shader->SetDepth((DepthComparisonType)m_depthType, true);

	theRenderer->SetShader(m_shader);

	int programHandle = m_shader->GetProgram()->GetHandle();
	theRenderer->SetVector4Uniform(programHandle, "LERPCOLOR", Rgba::ConvertToVector4(lerpColor));
	
	theRenderer->DrawMesh(m_mesh, m_transform.GetWorldMatrix());

	//special cases cleanup after draw
	m_shader->ResetRenderState();

	theRenderer = nullptr;
}

