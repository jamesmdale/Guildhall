#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "engine/core/Rgba.hpp"
#include <string>
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Item.hpp"

Entity::Entity(EntityDefinition* entityDef)
{
	m_entityDef = entityDef;

	m_spriteAnimSet = new SpriteAnimSet(m_entityDef->m_animSetDef);
	if(m_entityDef->m_startingTags != "")
	{
		m_tags.SetOrRemoveTags(m_entityDef->m_startingTags);
	}
}
void Entity::Update(float timeDelta)
{
	float moveSpeed = 0.0f;

	//TODO: will need to come back and adjust the movement speeds based on what type of tile they are on.
	if(m_entityDef->m_doesWalk)
	{
		moveSpeed = m_entityDef->m_walkSpeed;
	}
	if(m_entityDef->m_doesFly)
	{
		moveSpeed = m_entityDef->m_flySpeed;
	}
	m_center += (m_movementVector * (moveSpeed * timeDelta));
	
	//TODO: handle basic movement animations. CurrentAnim handles missing cases.
	if(m_movementVector == g_eastVector2)
	{
		m_spriteAnimSet->SetCurrentAnim("MoveEast");
	}
	else if(m_movementVector == g_westVector2)
	{
		m_spriteAnimSet->SetCurrentAnim("MoveWest");
	}
	else if(m_movementVector == g_northVector2)
	{
		m_spriteAnimSet->SetCurrentAnim("MoveNorth");
	}
	else if(m_movementVector == g_southVector2)
	{
		m_spriteAnimSet->SetCurrentAnim("MoveSouth");
	}
	else
	{		
		m_spriteAnimSet->SetCurrentAnim("Idle");
		m_spriteAnimSet->SetCurrentAnim("WorldIdle");
	}
	m_spriteAnimSet->Update(timeDelta);

	m_age += timeDelta;
	if(m_health <= 0)
	{
		m_health = m_maxHealth;
		m_isDead = true;
	}
}

void Entity::Render()
{
	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_center);
	g_theRenderer->Scalef(1.f, 1.f, 1.f);

	g_theRenderer->DrawTexturedAABB(m_entityDef->m_localDrawBounds, m_spriteAnimSet->GetCurrentTexture(), m_spriteAnimSet->GetCurrentUVs().mins, m_spriteAnimSet->GetCurrentUVs().maxs, Rgba(255, 255, 255, 255)); 

	//TODO: fix drawText2D error
	//g_theRenderer->DrawText2D(Vector2(m_center.x, m_center.y + 1), m_entityDef->m_name, 5.f, g_defaultColor, 1.f, g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	g_theRenderer->PopMatrix();	
}