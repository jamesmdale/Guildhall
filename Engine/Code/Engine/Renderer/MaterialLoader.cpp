#include "Engine\Renderer\MaterialLoader.hpp"
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Renderer\ShaderProgram.hpp"

MaterialLoader::MaterialLoader(const tinyxml2::XMLElement& element)
{
	m_id = ParseXmlAttribute(element, "name", m_id);
	GUARANTEE_OR_DIE(m_id != "", "LOAD MATERIAL FROM FILE FAILED : ID INVALID");

	const tinyxml2::XMLElement* shaderElement = element.FirstChildElement("shader");
	if(shaderElement)
	{
		m_shaderId = ParseXmlAttribute(*shaderElement, "name", m_shaderId);
		GUARANTEE_OR_DIE(m_shaderId != "", "LOAD SHADER FROM FILE FAILED : ID INVALID");
	}

	const tinyxml2::XMLElement* textureElement = element.FirstChildElement("textures");
	if(textureElement)
	{
		for(const tinyxml2::XMLElement* textureNode = textureElement->FirstChildElement(); textureNode; textureNode = textureNode->NextSiblingElement())
		{	
			TextureInfo textureInfo;
			textureInfo.m_binding = ParseXmlAttribute(*textureNode, "bind", textureInfo.m_binding);
			textureInfo.m_filePath = ParseXmlAttribute(*textureNode, "file", textureInfo.m_filePath);

			m_textureInfo.push_back(textureInfo);
		}
	}
}

Material* MaterialLoader::LoadMaterialFromXML(const std::string& filePath)
{
	Renderer* theRenderer = Renderer::GetInstance();

	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	MaterialLoader loader = MaterialLoader(*pRoot);
	Material* material = loader.CreateMaterialFromDefinition();

	theRenderer = nullptr;
	return material;	
}

Material* MaterialLoader::CreateMaterialFromDefinition()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Material* material = new Material(theRenderer->CreateOrGetShader(m_shaderId));
	
	for (TextureInfo info : m_textureInfo)
	{
		Texture* texture = theRenderer->CreateOrGetTexture(info.m_filePath);
		material->SetTexture(info.m_binding, texture);
		material->SetSampler(info.m_binding, theRenderer->m_defaultSampler);
	}

	theRenderer = nullptr;

	return material;
}