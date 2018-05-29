#pragma once
#include "Game/EntityDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"


std::map< std::string, EntityDefinition* > EntityDefinition:: s_entityDefinitions;

EntityDefinition::EntityDefinition( const tinyxml2::XMLElement& element, Renderer& renderer)
{	
	m_name = ParseXmlAttribute(element, "name", m_name);
	m_startingTags = ParseXmlAttribute(element, "startingTags", m_startingTags);
	
	//parse size
	const tinyxml2::XMLElement* sizeElement = element.FirstChildElement("Size");
	if(sizeElement)
	{
		m_physicsRadius = ParseXmlAttribute(*sizeElement, "physicsRadius", m_physicsRadius);
		m_localDrawBounds = ParseXmlAttribute(*sizeElement, "localDrawBounds", m_localDrawBounds); //TODO:draw with this if it exists
		if(m_localDrawBounds.mins == Vector2(0,0) && m_localDrawBounds.maxs == Vector2(0,0))
		{
			m_drawRadius = ParseXmlAttribute(*sizeElement, "drawRadius", m_drawRadius); //TODO:draw with this if it exists
			m_localDrawBounds = AABB2(Vector2(0,0), m_drawRadius, m_drawRadius);
		}

		m_drawRadius = ParseXmlAttribute(*sizeElement, "drawRadius", m_drawRadius); //TODO:draw with this if it exists
	}

	//parse movement
	const tinyxml2::XMLElement* movementElement = element.FirstChildElement("Movement");
	if(movementElement)
	{
		const tinyxml2::XMLElement* walkElement = movementElement->FirstChildElement("Walk");
		if(walkElement)
		{
			m_walkSpeed = ParseXmlAttribute(*walkElement, "speed", m_walkSpeed);
			m_doesWalk = true;
		}

		const tinyxml2::XMLElement* flyElement = movementElement->FirstChildElement("Fly");
		if(flyElement)
		{
			m_flySpeed = ParseXmlAttribute(*flyElement, "speed", m_flySpeed);
			m_doesFly = true;
		}
	}

	//parse sprite anim set
	const tinyxml2::XMLElement* spriteAnimElement = element.FirstChildElement("SpriteAnimSet");
	if(spriteAnimElement)
	{	
		m_animSetDef = new SpriteAnimSetDefinition(*spriteAnimElement, renderer);		
	} 
}

void EntityDefinition::Initialize(const std::string& filePath,  Renderer& renderer)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for(const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		EntityDefinition* newDef = new EntityDefinition(*definitionNode, renderer);		
		s_entityDefinitions.insert(std::pair<std::string, EntityDefinition*>(newDef->m_name, newDef));
	}	

	//debugger notification
	DebuggerPrintf("Loaded actor definitions!!!");
}
