#pragma once
#include "Game/TileDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

SpriteSheet* TileDefinition::s_tileSpriteSheet;
std::map< std::string, TileDefinition* > TileDefinition:: s_definitions;

TileDefinition::TileDefinition( const tinyxml2::XMLElement& element )
{	
	m_Name = ParseXmlAttribute(element, "name", m_Name);
	m_baseSpriteCoords = ParseXmlAttribute(element, "baseSpriteCoords", m_baseSpriteCoords);
	m_baseSpriteTint = ParseXmlAttribute(element, "baseSpriteTint", m_baseSpriteTint);
	
	m_overlaySpriteCoords = ParseXmlAttribute(element, "overlaySpriteCoords", m_overlaySpriteCoords);
	m_overlaySpriteTint = ParseXmlAttribute(element, "overlaySpriteTint", m_overlaySpriteTint);

	m_allowSight = ParseXmlAttribute(element, "allowsSight", m_allowSight);
	m_allowsWalking = ParseXmlAttribute(element, "allowsWalking", m_allowsWalking);
	m_allowsFlying = ParseXmlAttribute(element, "allowsFlying", m_allowsFlying);
	m_allowsSwimming = ParseXmlAttribute(element, "allowsSwimming", m_allowsSwimming);
	m_baseSpriteUVCoords = g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_baseSpriteCoords);

	if(m_overlaySpriteCoords != IntVector2(-1,-1))
	{
		m_overlaySpriteUVCoords = g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_overlaySpriteCoords);
	}

	s_tileSpriteSheet = g_tileSpriteSheet;	
}

void TileDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		TileDefinition* newDef = new TileDefinition(*definitionNode);		
		s_definitions.insert(std::pair<std::string, TileDefinition*>(std::string(newDef->m_Name), newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded tile definitions!!!");
}
