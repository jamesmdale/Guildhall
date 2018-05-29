#pragma once
#include <vector>
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include <string>
#include "Engine\Renderer\Material.hpp"

struct TextureInfo
{
	int m_binding = -1;
	std::string m_filePath = "default";
};


class MaterialLoader
{
public:
	explicit MaterialLoader( const tinyxml2::XMLElement& element);
	static Material* LoadMaterialFromXML(const std::string& filePath);
	Material* CreateMaterialFromDefinition();

public:
	std::string m_id = "";
	std::string m_shaderId = "default";

	std::vector<TextureInfo> m_textureInfo;
};



