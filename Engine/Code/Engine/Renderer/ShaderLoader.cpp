#include "Engine\Renderer\ShaderLoader.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"



ShaderLoader::ShaderLoader(const tinyxml2::XMLElement& element)
{
	m_id = ParseXmlAttribute(element, "name", m_id);
	GUARANTEE_OR_DIE(m_id != "", "LOAD SHADER FROM FILE FAILED : ID INVALID");

	m_cullMode = ParseXmlAttribute(element, "cull", m_cullMode);
	m_drawMode = ParseXmlAttribute(element, "drawmode", m_drawMode);
	m_drawModeFace = ParseXmlAttribute(element, "drawface", m_drawModeFace);
	m_windOrder = ParseXmlAttribute(element, "wind", m_windOrder);

	const tinyxml2::XMLElement* programElement = element.FirstChildElement("program");
	if(programElement) 
	{
		m_programId = ParseXmlAttribute(*programElement, "id", m_programId);

		if(m_programId == "")
		{
			m_programId = m_id;
		}
		
		const tinyxml2::XMLElement* vertexElement = programElement->FirstChildElement("vertex");
		if(vertexElement) 
		{
			m_vertexFilePath = ParseXmlAttribute(*vertexElement, "file", m_vertexFilePath);
			GUARANTEE_OR_DIE(m_vertexFilePath != "", "LOAD SHADER FROM FILE FAILED : VERTEX FILE PATH INVALID");
		}

		const tinyxml2::XMLElement* fragmentElement = programElement->FirstChildElement("fragment");
		if(fragmentElement) 
		{
			m_fragmentFilePath = ParseXmlAttribute(*fragmentElement, "file", m_fragmentFilePath);
			GUARANTEE_OR_DIE(m_fragmentFilePath != "", "LOAD SHADER FROM FILE FAILED : FRAGMENT FILE PATH INVALID");
		}
	}

	const tinyxml2::XMLElement* lightingElement = element.FirstChildElement("lighting");
	if(lightingElement) 
	{
		m_doesUseLighting = ParseXmlAttribute(*lightingElement, "lit", m_doesUseLighting);
	}

	const tinyxml2::XMLElement* orderElement = element.FirstChildElement("order");
	if(orderElement) 
	{
		m_sortLayer = ParseXmlAttribute(*orderElement, "sortLayer", m_sortLayer);
		m_renderQueue = ParseXmlAttribute(*orderElement, "renderQueue", m_renderQueue);
	}


	const tinyxml2::XMLElement* blendElement = element.FirstChildElement("blend");
	if(blendElement) 
	{
		const tinyxml2::XMLElement* colorElement = blendElement->FirstChildElement("color");
		if(colorElement)
		{
			m_colorBlendOperation = ParseXmlAttribute(*colorElement, "op", m_colorBlendOperation);
			m_colorSourceFactor = ParseXmlAttribute(*colorElement, "src", m_colorSourceFactor);
			m_colorDestinationFactor = ParseXmlAttribute(*colorElement, "dest", m_colorDestinationFactor);
		}

		const tinyxml2::XMLElement* alphaElement = blendElement->FirstChildElement("alpha");
		if(alphaElement)
		{
			m_alphaBlendOperation = ParseXmlAttribute(*alphaElement, "op", m_alphaBlendOperation);
			m_alphaSourceFactor = ParseXmlAttribute(*alphaElement, "src", m_alphaSourceFactor);
			m_alphaDestinationFactor = ParseXmlAttribute(*alphaElement, "dest", m_alphaDestinationFactor);
		}
	}

	const tinyxml2::XMLElement* depthElement = element.FirstChildElement("depth");
	if(depthElement) 
	{
		m_depthWrite = ParseXmlAttribute(*depthElement, "write", m_depthWrite);
		m_depthCompare = ParseXmlAttribute(*depthElement, "compare", m_depthCompare);
	}

	const tinyxml2::XMLElement* bindingsElement = element.FirstChildElement("bindings");
	if(bindingsElement)
	{
		for(const tinyxml2::XMLElement* bindingNode = bindingsElement->FirstChildElement(); bindingNode; bindingNode = bindingNode->NextSiblingElement())
		{	
			Binding binding;
			binding.m_dataType = ParseXmlAttribute(*bindingNode, "type", binding.m_dataType);
			binding.m_bindingName = ParseXmlAttribute(*bindingNode, "bind", binding.m_bindingName);
			binding.m_value = ParseXmlAttribute(*bindingNode, "value", binding.m_dataType);

			m_bindings.push_back(binding);
		}
	}
}

Shader* ShaderLoader::LoadShaderFromXML(const std::string& filePath)
{
	Renderer* theRenderer = Renderer::GetInstance();

	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	ShaderLoader loader = ShaderLoader(*pRoot);
	Shader* shader = loader.CreateShaderFromDefinition();

	theRenderer->m_loadedShaders.insert(std::pair<std::string, Shader*>(shader->m_id, shader));

	theRenderer = nullptr;
	return shader;	
}

Shader* ShaderLoader::CreateShaderFromDefinition()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Shader* shader = new Shader();

	shader->m_id = m_id;
	shader->m_program = theRenderer->CreateOrGetShaderProgramFromSeparatePaths(m_programId, m_vertexFilePath, m_fragmentFilePath);

	//set shader states (defaults to shader default)
	
	//sorting layer
	shader->m_sortingLayer = m_sortLayer;
			
	//render queue
	if(m_renderQueue == "opaque")
		shader->m_renderQueueType = RENDER_QUEUE_OPAQUE;
	else if(m_renderQueue == "additive")
		shader->m_renderQueueType = RENDER_QUEUE_ADDITIVE;
	else if(m_renderQueue == "alpha")
		shader->m_renderQueueType = RENDER_QUEUE_ALPHA;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: RENDER QUEUE INVALID FOR SHADER: %s", m_id.c_str()));

	//does use lighting
	shader->m_doesUseLighting = m_doesUseLighting;

	//cullmode
	if(m_cullMode == "back")
		shader->m_state.m_cullMode = CULL_MODE_BACK;
	else if(m_cullMode == "front")
		shader->m_state.m_cullMode = CULL_MODE_FRONT;
	else if(m_cullMode == "none")
		shader->m_state.m_cullMode = CULL_MODE_NONE;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: CULL MODE INVALID FOR SHADER: %s", m_id.c_str()));

	//draw mode
	if(m_drawMode == "fill")
		shader->m_state.m_drawMode = FILL_DRAW_MODE;
	else if(m_drawMode == "wire")
		shader->m_state.m_drawMode = LINE_DRAW_MODE;
	else if(m_drawMode == "point")
		shader->m_state.m_drawMode = POINT_DRAW_MODE;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: DRAW MODE INVALID FOR SHADER: %s", m_id.c_str()));

	//draw mode face
	if(m_drawModeFace == "front")
		shader->m_state.m_drawModeFace = FRONT_FACE_MODE;
	else if(m_drawModeFace == "back")
		shader->m_state.m_drawModeFace = BACK_FACE_MODE;
	else if(m_drawModeFace == "front_and_back")
		shader->m_state.m_drawModeFace = FRONT_AND_BACK_FACE_MODE;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: DRAAW MODE FACE INVALID FOR SHADER: %s", m_id.c_str()));

	//wind order mode
	if(m_windOrder == "ccw")
		shader->m_state.m_windOrder = WIND_COUNTER_CLOCKWISE;
	else if(m_windOrder == "cw")
		shader->m_state.m_windOrder = WIND_CLOCKWISE;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: WIND ORDER INVALID FOR SHADER: %s", m_id.c_str()));

	//blend color op
	if(m_colorBlendOperation == "add")
		shader->m_state.m_colorBlendOperation = BLEND_OP_ADD;
	else if(m_colorBlendOperation == "sub")
		shader->m_state.m_colorBlendOperation = BLEND_OP_SUBTRACT;
	else if(m_colorBlendOperation == "rev_sub")
		shader->m_state.m_colorBlendOperation = BLEND_OP_REVERSE_SUBTRACT;
	else if(m_colorBlendOperation == "min")
		shader->m_state.m_colorBlendOperation = BLEND_OP_MIN;
	else if(m_colorBlendOperation == "max")
		shader->m_state.m_colorBlendOperation = BLEND_OP_MAX;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: BLEND OP INVALID FOR SHADER: %s", m_id.c_str()));

	//blend color factor source
	if(m_colorSourceFactor == "zero")
		shader->m_state.m_colorSourceFactor = BLEND_ZERO;
	else if(m_colorSourceFactor == "one")
		shader->m_state.m_colorSourceFactor = BLEND_ONE;
	else if(m_colorSourceFactor == "src_color")
		shader->m_state.m_colorSourceFactor = BLEND_SOURCE_COLOR;
	else if(m_colorSourceFactor == "inv_src_color")
		shader->m_state.m_colorSourceFactor = BLEND_ONE_MINUS_SOURCE_COLOR;
	else if(m_colorSourceFactor == "src_alpha")
		shader->m_state.m_colorSourceFactor = BLEND_SOURCE_ALPHA;
	else if(m_colorSourceFactor == "inv_src_alpha")
		shader->m_state.m_colorSourceFactor = BLEND_ONE_MINUS_SOURCE_ALPHA;
	else if(m_colorSourceFactor == "dest_color")
		shader->m_state.m_colorSourceFactor = BLEND_DESTINATION_COLOR;
	else if(m_colorSourceFactor == "inv_dest_color")
		shader->m_state.m_colorSourceFactor = BLEND_ONE_MINUS_DESTINATION_COLOR;
	else if(m_colorSourceFactor == "dest_alpha")
		shader->m_state.m_colorSourceFactor = BLEND_DESTINATION_ALPHA;
	else if(m_colorSourceFactor == "inv_dest_alpha")
		shader->m_state.m_colorSourceFactor = BLEND_ONE_MINUS_DESTINATION_ALPHA;
	else if(m_colorSourceFactor == "constant_color")
		shader->m_state.m_colorSourceFactor = BLEND_CONSTANT_COLOR;
	else if(m_colorSourceFactor == "inv_constant_color")
		shader->m_state.m_colorSourceFactor = BLEND_ONE_MINUS_CONSTANT_COLOR;
	else if(m_colorSourceFactor == "const_alpha")
		shader->m_state.m_colorSourceFactor = BLEND_CONSTANT_ALPHA;
	else if(m_colorSourceFactor == "inv_const_alpha")
		shader->m_state.m_colorSourceFactor = BLEND_ONE_MINUS_CONSTANT_ALPHA;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: BLEND OP SRC INVALID FOR SHADER: %s", m_id.c_str()));

	//blend color factor destination
	if(m_colorDestinationFactor == "zero")
		shader->m_state.m_colorDestinationFactor = BLEND_ZERO;
	else if(m_colorDestinationFactor == "one")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE;
	else if(m_colorDestinationFactor == "src_color")
		shader->m_state.m_colorDestinationFactor = BLEND_SOURCE_COLOR;
	else if(m_colorDestinationFactor == "inv_src_color")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE_MINUS_SOURCE_COLOR;
	else if(m_colorDestinationFactor == "src_alpha")
		shader->m_state.m_colorDestinationFactor = BLEND_SOURCE_ALPHA;
	else if(m_colorDestinationFactor == "inv_src_alpha")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE_MINUS_SOURCE_ALPHA;
	else if(m_colorDestinationFactor == "dest_color")
		shader->m_state.m_colorDestinationFactor = BLEND_DESTINATION_COLOR;
	else if(m_colorDestinationFactor == "inv_dest_color")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE_MINUS_DESTINATION_COLOR;
	else if(m_colorDestinationFactor == "dest_alpha")
		shader->m_state.m_colorDestinationFactor = BLEND_DESTINATION_ALPHA;
	else if(m_colorDestinationFactor == "inv_dest_alpha")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE_MINUS_DESTINATION_ALPHA;
	else if(m_colorDestinationFactor == "constant_color")
		shader->m_state.m_colorDestinationFactor = BLEND_CONSTANT_COLOR;
	else if(m_colorDestinationFactor == "inv_constant_color")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE_MINUS_CONSTANT_COLOR;
	else if(m_colorDestinationFactor == "const_alpha")
		shader->m_state.m_colorDestinationFactor = BLEND_CONSTANT_ALPHA;
	else if(m_colorDestinationFactor == "inv_const_alpha")
		shader->m_state.m_colorDestinationFactor = BLEND_ONE_MINUS_CONSTANT_ALPHA;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: BLEND OP DEST INVALID FOR SHADER: %s", m_id.c_str()));

	//blend alpha operation
	if(m_alphaBlendOperation == "add")
		shader->m_state.m_alphaBlendOperation = BLEND_OP_ADD;
	else if(m_alphaBlendOperation == "sub")
		shader->m_state.m_alphaBlendOperation = BLEND_OP_SUBTRACT;
	else if(m_alphaBlendOperation == "rev_sub")
		shader->m_state.m_alphaBlendOperation = BLEND_OP_REVERSE_SUBTRACT;
	else if(m_alphaBlendOperation == "min")
		shader->m_state.m_alphaBlendOperation = BLEND_OP_MIN;
	else if(m_alphaBlendOperation == "max")
		shader->m_state.m_alphaBlendOperation = BLEND_OP_MAX;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: ALPHA BLEND OP INVALID FOR SHADER: %s", m_id.c_str()));

	//blend alpha factor source
	if(m_alphaSourceFactor == "zero")
		shader->m_state.m_alphaSourceFactor = BLEND_ZERO;
	else if(m_alphaSourceFactor == "one")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE;
	else if(m_alphaSourceFactor == "src_color")
		shader->m_state.m_alphaSourceFactor = BLEND_SOURCE_COLOR;
	else if(m_alphaSourceFactor == "inv_src_color")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE_MINUS_SOURCE_COLOR;
	else if(m_alphaSourceFactor == "src_alpha")
		shader->m_state.m_alphaSourceFactor = BLEND_SOURCE_ALPHA;
	else if(m_alphaSourceFactor == "inv_src_alpha")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE_MINUS_SOURCE_ALPHA;
	else if(m_alphaSourceFactor == "dest_color")
		shader->m_state.m_alphaSourceFactor = BLEND_DESTINATION_COLOR;
	else if(m_alphaSourceFactor == "inv_dest_color")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE_MINUS_DESTINATION_COLOR;
	else if(m_alphaSourceFactor == "dest_alpha")
		shader->m_state.m_alphaSourceFactor = BLEND_DESTINATION_ALPHA;
	else if(m_alphaSourceFactor == "inv_dest_alpha")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE_MINUS_DESTINATION_ALPHA;
	else if(m_alphaSourceFactor == "constant_color")
		shader->m_state.m_alphaSourceFactor = BLEND_CONSTANT_COLOR;
	else if(m_alphaSourceFactor == "inv_constant_color")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE_MINUS_CONSTANT_COLOR;
	else if(m_alphaSourceFactor == "const_alpha")
		shader->m_state.m_alphaSourceFactor = BLEND_CONSTANT_ALPHA;
	else if(m_alphaSourceFactor == "inv_const_alpha")
		shader->m_state.m_alphaSourceFactor = BLEND_ONE_MINUS_CONSTANT_ALPHA;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: ALPHA BLEND SRC INVALID FOR SHADER: %s", m_id.c_str()));

	//blend alpha factor destination
	if(m_alphaDestinationFactor == "zero")
		shader->m_state.m_alphaDestinationFactor = BLEND_ZERO;
	else if(m_alphaDestinationFactor == "one")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE;
	else if(m_alphaDestinationFactor == "src_color")
		shader->m_state.m_alphaDestinationFactor = BLEND_SOURCE_COLOR;
	else if(m_alphaDestinationFactor == "inv_src_color")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_SOURCE_COLOR;
	else if(m_alphaDestinationFactor == "src_alpha")
		shader->m_state.m_alphaDestinationFactor = BLEND_SOURCE_ALPHA;
	else if(m_alphaDestinationFactor == "inv_src_alpha")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_SOURCE_ALPHA;
	else if(m_alphaDestinationFactor == "dest_color")
		shader->m_state.m_alphaDestinationFactor = BLEND_DESTINATION_COLOR;
	else if(m_alphaDestinationFactor == "inv_dest_color")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_DESTINATION_COLOR;
	else if(m_alphaDestinationFactor == "dest_alpha")
		shader->m_state.m_alphaDestinationFactor = BLEND_DESTINATION_ALPHA;
	else if(m_alphaDestinationFactor == "inv_dest_alpha")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_DESTINATION_ALPHA;
	else if(m_alphaDestinationFactor == "constant_color")
		shader->m_state.m_alphaDestinationFactor = BLEND_CONSTANT_COLOR;
	else if(m_alphaDestinationFactor == "inv_constant_color")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_CONSTANT_COLOR;
	else if(m_alphaDestinationFactor == "const_alpha")
		shader->m_state.m_alphaDestinationFactor = BLEND_CONSTANT_ALPHA;
	else if(m_alphaDestinationFactor == "inv_const_alpha")
		shader->m_state.m_alphaDestinationFactor = BLEND_ONE_MINUS_CONSTANT_ALPHA;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: ALPHA BLEND DEST INVALID FOR SHADER: %s", m_id.c_str()));

	//depth write
	shader->m_state.m_depthWrite = m_depthWrite;

	//depth test
	if(m_depthCompare == "less")
		shader->m_state.m_depthCompare = LESS_DEPTH_TYPE;
	else if(m_depthCompare == "never")
		shader->m_state.m_depthCompare = NEVER_DEPTH_TYPE;
	else if(m_depthCompare == "equal")
		shader->m_state.m_depthCompare = EQUAL_DEPTH_TYPE;
	else if(m_depthCompare == "lequal")
		shader->m_state.m_depthCompare = LESS_THAN_OR_EQUAL_DEPTH_TYPE;
	else if(m_depthCompare == "greater")
		shader->m_state.m_depthCompare = GREATER_DEPTH_TYPE;
	else if(m_depthCompare == "gequal")
		shader->m_state.m_depthCompare = GREATER_THAN_OR_EQUAL_DEPTH_TYPE;
	else if(m_depthCompare == "not")
		shader->m_state.m_depthCompare = NOT_EQUAL_DEPTH_TYPE;
	else if(m_depthCompare == "always")
		shader->m_state.m_depthCompare = ALWAYS_DEPTH_TYPE;
	else
		GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: DEPTH TYPE INVALID FOR SHADER: %s", m_id.c_str()));

	for (Binding binding : m_bindings)
	{
		if(binding.m_bindingName == "" || binding.m_dataType == "" || binding.m_value == "")
		{
			GUARANTEE_OR_DIE(false, Stringf("XML SHADER LOAD ERROR: CUSTOM BINDING %s INVALID FOR SHADER: %s", binding.m_bindingName.c_str(), m_id.c_str()));
		}
		else
		{
			shader->m_bindings.push_back(binding);
		}		
	}

	return shader;
}
