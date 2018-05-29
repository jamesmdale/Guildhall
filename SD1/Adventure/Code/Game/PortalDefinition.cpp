#pragma once
#include "Game/PortalDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"


std::map< std::string, PortalDefinition* > PortalDefinition:: s_portalDefinitions;

PortalDefinition::PortalDefinition( const tinyxml2::XMLElement& element, Renderer& renderer) : EntityDefinition(element, renderer)
{	

}

void PortalDefinition::Initialize(const std::string& filePath, Renderer& renderer)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		PortalDefinition* newDef = new PortalDefinition(*definitionNode, renderer);		
		s_portalDefinitions.insert(std::pair<std::string, PortalDefinition*>(newDef->m_name, newDef));
		//s_entityDefinitions.insert(std::pair<std::string, PortalDefinition*>(newDef->m_name, newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded portal definitions!!!");
}
