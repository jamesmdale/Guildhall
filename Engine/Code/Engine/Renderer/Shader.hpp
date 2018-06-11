#pragma once
#include "Engine\Core\EngineCommon.hpp"
#include <string>
#include "Engine\Renderer\RendererTypes.hpp"
#include <vector>
#include "Engine\Renderer\MaterialProperty.hpp"

class ShaderProgram;

struct Binding
{
	std::string m_dataType = "";  //example "rgba"
	std::string m_bindingName = ""; //example "TINT"
	std::string m_value = ""; //example (1.f,1.f,1.f,1.f);
};

enum RenderQueueType
{
	RENDER_QUEUE_ALPHA,	
	RENDER_QUEUE_ADDITIVE,
	RENDER_QUEUE_OPAQUE,
	NUM_RENDER_QUEUE_TYPES
};

struct RenderState
{
	 //Raster State Control
	CullModeType m_cullMode;// = CULL_MODE_BACK;						// CULL_BACK
	WindOrderType m_windOrder; //= WIND_COUNTER_CLOCKWISE;			// WIND_COUNTER_CLOCKWISE

	DrawModeType m_drawMode; //= FILL_DRAW_MODE;						// FILL_SOLID
	DrawModeFaceType m_drawModeFace; //= FRONT_AND_BACK_FACE_MODE;

	// Depth State Control
	DepthComparisonType m_depthCompare; //= LESS_DEPTH_TYPE;			// COMPARE_LESS
	bool m_depthWrite = true;										// true

	// Blend control
	BlendOperationType m_colorBlendOperation;// = BLEND_OP_ADD;		// COMPARE_ADD
	BlendFactorType m_colorSourceFactor; //= BLEND_ONE;				// BLEND_ONE
	BlendFactorType m_colorDestinationFactor; //= BLEND_ZERO;			// BLEND_ZERO

	BlendOperationType m_alphaBlendOperation; //= BLEND_OP_ADD;		// COMPARE_ADD
	BlendFactorType m_alphaSourceFactor; //= BLEND_ONE;				// BLEND_ONE
	BlendFactorType m_alphaDestinationFactor; //= BLEND_ONE;			// BLEND_ONE
};

class Shader
{
public:
	Shader();
	~Shader();
	Shader(const Shader* copyShader);

	/************************************************************************/
	/* Setters                                                              */
	/************************************************************************/
	void SetProgram(ShaderProgram* program);
	
	void EnableColorBlending(BlendOperationType operationType, BlendFactorType source, BlendFactorType destination);
	void EnableAlphaBlending(BlendOperationType operationType, BlendFactorType sourceAlpha, BlendFactorType destinationAlpha);
	void EnableCompleteBlending(BlendOperationType operationTypeColor, BlendFactorType sourceColor, BlendFactorType destinationColor, BlendOperationType operationTypeAlpha, BlendFactorType sourceAlpha, BlendFactorType destinationAlpha);
	void DisableBlending();

	void SetDepth(DepthComparisonType compareType, bool shouldWrite);
	void DisableDepth();
	void SetCullMode(CullModeType mode);
	void SetFillMode(DrawModeType mode);
	void SetFrontFace(WindOrderType order);

	void ResetRenderState();	

	/************************************************************************/
	/* Getters                                                              */
	/************************************************************************/
	ShaderProgram* GetProgram(){return m_program;}
	int GetSortingLayer(){return m_sortingLayer;}
	RenderQueueType GetRenderQueueType(){return m_renderQueueType;}

	static Shader* Clone(Shader* shaderToClone);


public:
	std::string m_id;
	ShaderProgram* m_program;
	RenderState m_state;
	int m_sortingLayer;
	RenderQueueType m_renderQueueType;
	bool m_doesUseLighting = false;
	std::vector<Binding> m_bindings;
};


