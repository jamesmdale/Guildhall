#include "Engine\Renderer\Shader.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\ShaderProgram.hpp"
#include "Engine\Core\EngineCommon.hpp"

Shader::Shader()
{
	m_program = nullptr;
	ResetRenderState();	
}

Shader::~Shader()
{
	TODO("Make sure that this data is instanced before deleting");
	//delete(m_program);
	m_program = nullptr;
}

Shader::Shader(const Shader * copyShader)
{
	m_program = new ShaderProgram(copyShader->m_program);
	m_id = copyShader->m_id;
	m_state = copyShader->m_state;
	m_sortingLayer = copyShader->m_sortingLayer;
	m_renderQueueType = copyShader->m_renderQueueType;
	m_doesUseLighting = copyShader->m_doesUseLighting;
	
	for (int bindingIndex = 0; bindingIndex < (int)copyShader->m_bindings.size(); ++bindingIndex)
	{
		m_bindings.push_back(copyShader->m_bindings[bindingIndex]);
	}
}

void Shader::SetProgram(ShaderProgram* program)
{
	m_program = program;
}

void Shader::EnableColorBlending(BlendOperationType operationType, BlendFactorType sourceColor, BlendFactorType destinationColor)
{
	m_state.m_colorBlendOperation = operationType;
	m_state.m_colorSourceFactor = sourceColor;
	m_state.m_colorDestinationFactor = destinationColor;
}

void Shader::EnableAlphaBlending(BlendOperationType operationType, BlendFactorType sourceAlpha, BlendFactorType destinationAlpha)
{
	m_state.m_alphaBlendOperation = operationType;
	m_state.m_alphaSourceFactor = sourceAlpha;
	m_state.m_alphaDestinationFactor = destinationAlpha;
}

void Shader::EnableCompleteBlending(BlendOperationType operationTypeColor, BlendFactorType sourceColor, BlendFactorType destinationColor, BlendOperationType operationTypeAlpha, BlendFactorType sourceAlpha, BlendFactorType destinationAlpha)
{
	EnableColorBlending( operationTypeColor,  sourceColor,  destinationColor);
	EnableAlphaBlending( operationTypeAlpha, sourceAlpha, destinationAlpha);
}

void Shader::DisableBlending()
{
	// Blend control
	m_state.m_colorBlendOperation = BLEND_OP_ADD;			// COMPARE_ADD
	m_state.m_colorSourceFactor = BLEND_ONE;				// BLEND_ONE
	m_state.m_colorDestinationFactor = BLEND_ZERO;			// BLEND_ZERO

	m_state.m_alphaBlendOperation = BLEND_OP_ADD;			// COMPARE_ADD
	m_state.m_alphaSourceFactor = BLEND_ONE;				// BLEND_ONE
	m_state.m_alphaDestinationFactor = BLEND_ONE;			// BLEND_ONE
}

void Shader::SetDepth(DepthComparisonType compareType, bool shouldWrite)
{
	m_state.m_depthCompare = compareType;
	m_state.m_depthWrite = shouldWrite;
}

void Shader::DisableDepth()
{
	SetDepth(ALWAYS_DEPTH_TYPE, false);
}

void Shader::SetCullMode(CullModeType mode)
{
	m_state.m_cullMode = mode;
}

void Shader::SetFillMode(DrawModeType mode)
{
	m_state.m_drawMode = mode;
}

void Shader::SetFrontFace(WindOrderType order)
{
	m_state.m_windOrder = order;
}

void Shader::ResetRenderState()
{
	m_state.m_cullMode = CULL_MODE_BACK;						// CULL_BACK
	m_state.m_windOrder = WIND_COUNTER_CLOCKWISE;			// WIND_COUNTER_CLOCKWISE

	m_state.m_drawMode = FILL_DRAW_MODE;						// FILL_SOLID
	m_state.m_drawModeFace = FRONT_AND_BACK_FACE_MODE;  
	
	// Depth State Control
	m_state.m_depthCompare = LESS_DEPTH_TYPE;			// COMPARE_LESS
	m_state.m_depthWrite = true;						// true

	// Blend control
	m_state.m_colorBlendOperation = BLEND_OP_ADD;			// COMPARE_ADD
	m_state.m_colorSourceFactor = BLEND_ONE;				// BLEND_ONE
	m_state.m_colorDestinationFactor = BLEND_ZERO;			// BLEND_ZERO

	m_state.m_alphaBlendOperation = BLEND_OP_ADD;			// COMPARE_ADD
	m_state.m_alphaSourceFactor = BLEND_SOURCE_ALPHA;				// BLEND_ONE
	m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_SOURCE_ALPHA;			// BLEND_ONE

	//sorting and alpha
	m_sortingLayer = 0;
	m_renderQueueType = RENDER_QUEUE_OPAQUE;
	m_bindings.clear();

	TODO("reset does use lighting");
}


Shader* Shader::Clone(Shader * shaderToClone)
{
	if(shaderToClone == nullptr)
	{
		return nullptr;
	}

	Shader* clonedShader = new Shader();
	memcpy(clonedShader, shaderToClone, sizeof(Shader));

	return clonedShader;
}
