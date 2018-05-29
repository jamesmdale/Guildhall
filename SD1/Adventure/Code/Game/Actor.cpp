#include "Game/Actor.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Actor::Actor(ActorDefinition* actorDef) : Entity((EntityDefinition*)actorDef)
{
	m_actorDef = actorDef;
	m_health = (float)actorDef->m_startHealth;

	for(int statIndex = 0; statIndex < NUM_STATS; statIndex++)
	{
		m_stats.stats[statIndex] = GetRandomIntInRange(m_actorDef->m_minStats.stats[statIndex], m_actorDef->m_maxStats.stats[statIndex]);
	}	

	for(size_t slotIndex = 0; slotIndex < NUM_EQUIPMENT_SLOTS; slotIndex++)
	{
		m_equippedItems.push_back(nullptr);
	}
}

//SHARED LOGIC
void Actor::Update(float timeDelta)
{
	if(m_entityDef->m_name == "Player")
	{
		UpdatePlayerInput(timeDelta);
	}
	else
	{
		UpdateEnemy(timeDelta);
	}

	if(!m_isReadyToAttack)
	{
		m_shootCooldown += timeDelta;
		if(m_shootCooldown >= g_actorShootCooldown) //TODO: currently use a global cooldown. Can later be data driven per actor.
		{
			m_isReadyToAttack = true;
			m_shootCooldown = 0.f;				
		}
	}

	bool doUpdateAnimation = true;
	const SpriteAnim* currentAnim = m_spriteAnimSet->GetCurrentAnim();
	std::string currentAnimName = currentAnim->GetName();
	if(currentAnimName == "AttackEast"
		|| currentAnimName == "AttackWest"
		|| currentAnimName == "AttackNorth"
		|| currentAnimName == "AttackSouth"
		|| currentAnimName == "CastEast"
		|| currentAnimName == "CastNorth"
		|| currentAnimName == "CastSouth"
		|| currentAnimName == "CastWest")
	{
		if(currentAnim->IsFinished() == false)
		{
			doUpdateAnimation = false;
		}
	}

	std::string animToSet = "";
	if(doUpdateAnimation)
	{		
		if(m_movementVector == g_eastVector2)
		{
			if(m_isActorTryingToAttack && m_isReadyToAttack)
			{			
				if(m_actorDef->m_attackType == "melee")
				{
					animToSet = "AttackEast";
				}
				if(m_actorDef->m_attackType == "cast")
				{
					animToSet = "CastEast";
				}			
			}
			else
			{
				animToSet = "MoveEast";
			}		
		}
		else if(m_movementVector == g_westVector2)
		{
			if(m_isActorTryingToAttack  && m_isReadyToAttack)
			{			
				if(m_actorDef->m_attackType == "melee")
				{
					animToSet = "AttackWest";
				}
				if(m_actorDef->m_attackType == "cast")
				{
					animToSet = "CastWest";
				}			
			}
			else
			{
				animToSet = "MoveWest";
			}
		}
		else if(m_movementVector == g_northVector2)
		{
			if(m_isActorTryingToAttack  && m_isReadyToAttack)
			{			
				if(m_actorDef->m_attackType == "melee")
				{
					animToSet = "AttackNorth";
				}
				if(m_actorDef->m_attackType == "cast")
				{
					animToSet = "CastNorth";
				}			
			}
			else
			{
				animToSet = "MoveNorth";
			}
		}
		else if(m_movementVector == g_southVector2)
		{
			if(m_isActorTryingToAttack && m_isReadyToAttack)
			{			
				if(m_actorDef->m_attackType == "melee")
				{
					animToSet = "AttackSouth";
				}
				if(m_actorDef->m_attackType == "cast")
				{
					animToSet = "CastSouth";
				}			
			}
			else
			{
				animToSet = "MoveSouth";
			}
		}
		else if(m_movementVector == g_idleVector2)
		{
			if(m_isActorTryingToAttack && m_isReadyToAttack)
			{
				if(m_actorDef->m_attackType == "melee")
				{
					animToSet = "AttackSouth";
				}
				if(m_actorDef->m_attackType == "cast")
				{
					animToSet = "CastSouth";
				}		
			}
			else
			{
				animToSet = "Idle";	
			}
		}
		else
		{
			animToSet = "Idle";	
		}

		if(currentAnimName != animToSet)
		{
			m_spriteAnimSet->SetCurrentAnim(animToSet);
		}
		
	}
	for(size_t itemIndex = 0; itemIndex < m_equippedItems.size(); itemIndex++)
	{
		if(m_equippedItems[itemIndex] != nullptr)
		{
			m_equippedItems[itemIndex]->m_center = m_center;
			if(doUpdateAnimation)
			{
				if(currentAnimName != animToSet)
				{
					m_equippedItems[itemIndex]->m_spriteAnimSet->SetCurrentAnim(animToSet);
				}
			}			
			m_equippedItems[itemIndex]->m_spriteAnimSet->Update(timeDelta);
		}
	}

	m_spriteAnimSet->Update(timeDelta);
	
	
	if(!m_isDead)
	{	
		if(m_isActorTryingToAttack && m_isReadyToAttack)
		{
			Attack();
		}		

		if(m_isActorTryingToUsePotion)
		{
			m_isActorTryingToUsePotion = false;
			bool didUseItem = false;

			for(size_t inventoryIndex = 0; inventoryIndex < m_inventory.size(); inventoryIndex++)
			{
				if(m_inventory[inventoryIndex]->m_entityDef->m_name == "Potion")
				{
					if(m_inventory[inventoryIndex]->m_currentUseCooldown == 0.f)
					{
						m_inventory[inventoryIndex]->m_remainingUses--;

						//TODO: could add more conditions here for various stats or variables to modify (speed, etc)
						if(m_inventory[inventoryIndex]->m_itemDef->m_effectedStatModifier == "health")
						{
							m_health += m_inventory[inventoryIndex]->m_itemDef->m_effectAmount;
							if(m_health > m_maxHealth)
							{
								m_health = m_maxHealth;
							}
							didUseItem = true;
						}
						if(m_inventory[inventoryIndex]->m_remainingUses == 0)
						{
							m_inventory.erase(m_inventory.begin() + inventoryIndex);
						}

						break;
					}
				}
			}
		}

		if(m_isTryingToDropEquipmentItem)
		{
			m_isTryingToDropEquipmentItem = false;

			for(size_t itemIndex = 0; itemIndex < m_equippedItems.size(); itemIndex++)
			{
				if(m_equippedItems[itemIndex] != nullptr)
				{

				}
			}
		}
	}

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
	if(m_wasJustHit)
	{
		m_wasJustHit = false;
		m_currentHitCooldown = m_secondsToShowHitDetection;
	}

	if(m_currentHitCooldown > 0.f)
	{
		m_currentHitCooldown -= timeDelta;

		if(m_currentHitCooldown < 0.f)
		{
			m_currentHitCooldown = 0.f;
		}
	}
	
	
	m_age += timeDelta;
	if(m_health <= 0)
	{
		m_health = m_maxHealth;
		m_isDead = true;

		if(m_isDead && m_entityDef->m_name == "Player")
		{
			g_isPlayerAlive = false;
		}
	}
}

void Actor::Render()
{
	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_center);
	g_theRenderer->Scalef(1.f, 1.f, 1.f);
	Rgba playerColor = Rgba(255, 255, 255, 255);

	if(m_currentHitCooldown > 0.f)
	{
		playerColor = Rgba(255, 0, 0, 255);
	}

	g_theRenderer->DrawTexturedAABB(m_entityDef->m_localDrawBounds, m_spriteAnimSet->GetCurrentTexture(), m_spriteAnimSet->GetCurrentUVs().mins, m_spriteAnimSet->GetCurrentUVs().maxs, playerColor); 

	//TODO: fix drawText2D error
	//g_theRenderer->DrawText2D(Vector2(m_center.x, m_center.y + 1), m_entityDef->m_name, 5.f, g_defaultColor, 1.f, g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	g_theRenderer->PopMatrix();	

	for(size_t itemIndex = 0; itemIndex < m_equippedItems.size(); itemIndex++)
	{
		if(m_equippedItems[itemIndex] != nullptr)
		{
			g_theRenderer->PushMatrix();	
			g_theRenderer->Translatef(m_center);
			g_theRenderer->Scalef(1.f, 1.f, 1.f);

			g_theRenderer->DrawTexturedAABB(m_equippedItems[itemIndex]->m_entityDef->m_localDrawBounds, m_equippedItems[itemIndex]->m_spriteAnimSet->GetCurrentTexture(), m_equippedItems[itemIndex]->m_spriteAnimSet->GetCurrentUVs().mins, m_equippedItems[itemIndex]->m_spriteAnimSet->GetCurrentUVs().maxs, Rgba(255, 255, 255, 255)); 

			g_theRenderer->PopMatrix();
		}		
	}
}

void Actor::Attack()
{
	Map* map = g_Game->m_currentMap;
	m_isReadyToAttack = false;

	//TODO: will later update this to take the types of projectiles the actor has access to and spwans those
	if(m_actorDef->m_attackType == "cast")
	{
		map->SpawnProjectileFromActor(this, "Fireball");
	}			

	if(m_actorDef->m_attackType == "melee")
	{
		m_isAttacking = true;
	}

}


//ENEMY SPECIFIC LOGIC

void Actor::UpdateEnemy(float timeDelta)
{
	//randomly move the enemy
	//get ints because we want them to only move in cardinals
	m_isActorTryingToAttack = false;
	m_goalCooldown += timeDelta;
	if(m_goalCooldown >= m_actorGoalCooldownInSeconds)
	{
		m_goalCooldown = 0.f;
		m_movementVector = Vector2((float)GetRandomIntInRange(-1, 1), (float)GetRandomIntInRange(-1, 1));
	}	

	//loop through the actors on the map and find the player
	//TODO: could refactor this later if slow and have a player class or something.
	size_t playerIndex = 0;
	bool playerFound = false;
	for(playerIndex; playerIndex < g_Game->m_currentMap->m_actors.size(); playerIndex++)
	{
		if(g_Game->m_currentMap->m_actors[playerIndex] != nullptr)
		{
			if(g_Game->m_currentMap->m_actors[playerIndex]->m_entityDef->m_name == "Player")
			{
				playerFound = true;
				break;
			}
		}		
	}
	if(playerFound == true)
	{
		Actor* playerActor = g_Game->m_currentMap->m_actors[playerIndex];
		bool doeshaveLineOfSight = HasLineOfSight(m_center, playerActor->m_center);
		if(doeshaveLineOfSight)
		{
			m_shootDirection = (playerActor->m_center - m_center).GetNormalized();
			m_isActorTryingToAttack = true;
		}
	}	
}

bool Actor::HasLineOfSight(const Vector2& startPos, const Vector2& endPos) const
{
	Vector2 raycastDisplacementToTarget = (endPos - startPos).GetNormalized();
	float distanceOfRaycast = GetDistance(startPos, endPos);
	RaycastResult result = Raycast(startPos, raycastDisplacementToTarget, distanceOfRaycast);

	if(result.didCollide == true)
	{
		return false;
	}
	else
	{
		return true;
	}	
}

RaycastResult Actor::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance ) const
{
	RaycastResult result;
	float numSteps = maxDistance * g_raycastStepsPerTile;	
	float singleStep = 1/g_raycastStepsPerTile;

	IntVector2 previousTileCoords = ConvertVector2ToIntVector2(startPos);

	for(float stepIndex = 0; stepIndex <= numSteps; stepIndex++)
	{
		Vector2 currentPosition = startPos + (direction * (stepIndex * singleStep));
		IntVector2 currentTileCoordinates = ConvertVector2ToIntVector2(currentPosition);

		int currentTileIndex = (g_Game->m_currentMap->m_dimensions.y * currentTileCoordinates.x) + currentTileCoordinates.y;
		if(!g_Game->m_currentMap->m_tiles[currentTileIndex].m_tileDef->m_allowSight)
		{			
			result.didCollide = true;
			result.collisionTile = currentTileCoordinates;
			result.collisionPosition = currentPosition;

			return result;
		}		
	}
	result.didCollide = false;
	return result;
}


//PLAYER SPECIFIC LOGIC
void Actor::UpdatePlayerInput(float timeDelta)
{
	m_isActorTryingToAttack = false;
	UNUSED(timeDelta);
	//may want controller support here in the future
	
	m_movementVector = Vector2(0,0);
	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_W) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_UP_ARROW))
	{
		m_movementVector = Vector2(0,1);
		//TODO: could do this if you wanted 8 way movement - m_movementVector.y = 1.f;
	}
	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_S) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_DOWN_ARROW))
	{
		m_movementVector = Vector2(0,-1);
		//TODO: could do this if you wanted 8 way movement - m_movementVector.y = -1.f;
	}
	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_A) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_LEFT_ARROW))
	{
		m_movementVector = Vector2(-1,0);
		//TODO: could do this if you wanted 8 way movement - m_movementVector.x = -1.f;
	}
	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_D) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_RIGHT_ARROW))
	{
		m_movementVector = Vector2(1,0);
		//TODO: could do this if you wanted 8 way movement - m_movementVector.x = 1.f;
	}
	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_SPACE))
	{
		m_isActorTryingToAttack = true;
	}
	else
	{
		m_isActorTryingToAttack = false;
	}

	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_H))
	{
		m_health = 99999;
	}

	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_J))
	{
		m_movementSpeed = m_movementSpeed * 5.f;
	}
	else
	{
		m_movementSpeed = ((EntityDefinition*)m_actorDef)->m_walkSpeed;
	}

	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_E))
	{
		m_isActorTryingToInteract = true;
	}
	
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_F))
	{
		m_isActorTryingToUsePotion = true;
	}

	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_I)) //print inventory to debugger
	{
		DebuggerPrintf("------------------------------------\n");
		for(size_t itemIndex = 0; itemIndex < m_inventory.size(); itemIndex++)
		{		
			std::string itemName = m_inventory[itemIndex]->m_entityDef->m_name;
			DebuggerPrintf("Inventory Item: ");
			DebuggerPrintf(itemName.c_str());
			DebuggerPrintf("\n");
		}
		DebuggerPrintf("------------------------------------\n");
	}
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_P)) //print equipmentslots to debugger
	{
		DebuggerPrintf("------------------------------------\n");
		for(size_t itemIndex = 0; itemIndex < m_equippedItems.size(); itemIndex++)
		{
			if(m_equippedItems[itemIndex] != nullptr)
			{
				std::string itemName = m_equippedItems[itemIndex]->m_entityDef->m_name;
				DebuggerPrintf("Equipment Item: ");
				DebuggerPrintf(itemName.c_str());
				DebuggerPrintf("\n");
			}			
		}
		DebuggerPrintf("------------------------------------\n");
	}

	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_X)) //print equipmentslots to debugger
	{
		m_isTryingToDropEquipmentItem = true;
	}

	//TODO: Interact keys, drop keys, use potion key

}
