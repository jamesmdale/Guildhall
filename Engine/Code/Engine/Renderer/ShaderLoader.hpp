#pragma once
#include "Engine\Math\Vector2.hpp"
#include <map>
#include <vector>
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2/tinyxml2.h"
#include "Engine\Renderer\Shader.hpp"
#include <string>

class ShaderLoader
{
public:
	explicit ShaderLoader( const tinyxml2::XMLElement& element);
	static Shader* LoadShaderFromXML(const std::string& filePath);
	Shader* CreateShaderFromDefinition();

public:
	std::string m_id = "";
	std::string m_cullMode = "back";
	std::string m_drawMode = "fill";
	std::string m_drawModeFace = "front_and_back";
	std::string m_windOrder = "ccw";
	std::string m_programId = "";
	std::string m_vertexFilePath = "";
	std::string m_fragmentFilePath = "";

	std::string m_colorBlendOperation = "add";
	std::string m_colorSourceFactor = "one";
	std::string m_colorDestinationFactor = "zero";

	std::string m_alphaBlendOperation = "add";
	std::string m_alphaSourceFactor = "one";
	std::string m_alphaDestinationFactor = "one";

	bool m_doesUseLighting = false;

	int m_sortLayer = 0;
	std::string m_renderQueue = "opaque";
	
	bool m_depthWrite = true;
	std::string m_depthCompare = "less";

	std::vector<Binding> m_bindings;
};



