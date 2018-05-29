#include "Game/Map.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/MapGenStep_FillAndEdge.hpp"
#include "game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Camera.hpp"
#include "Game/Stats.hpp"


Map::Map(MapDefinition* definition, const std::string& mapName)
{
	m_Name = mapName;
	m_mapDef = definition;

	int numTilesX = GetRandomIntInRange(m_mapDef->m_width.min, m_mapDef->m_width.max);
	int numTilesY = GetRandomIntInRange(m_mapDef->m_height.min, m_mapDef->m_height.max);

	m_dimensions = IntVector2(numTilesX, numTilesY);

	for(int xCoordinate = 0; xCoordinate < numTilesX; xCoordinate++)
	{	
		for(int yCoordinate = 0; yCoordinate < numTilesY; yCoordinate++)
		{
			Tile newTile;

			newTile.m_tileCoords = IntVector2(xCoordinate, yCoordinate);
			newTile.m_tileDef = TileDefinition::s_definitions[m_mapDef->m_defaultTile->m_Name];		
				
			m_tiles.push_back(newTile);
		}		
	}

	for(int genStepsIndex = 0; genStepsIndex < (int)definition->m_genSteps.size(); genStepsIndex++)
	{
		int iterations = GetRandomIntInRange(definition->m_iterations.min, definition->m_iterations.max);
		float chanceToRun = GetRandomFloatZeroToOne();
		if(chanceToRun <= definition->m_chanceToRun)
		{
			for(int iterationIndex = 0; iterationIndex < iterations; iterationIndex++)
			{
				definition->m_genSteps[genStepsIndex]->Run(*this);
			}	
		}			
	}
	m_camera = new Camera(g_cameraNumTilesViewHorizontally, g_cameraNumTilesViewVertically);
}

void Map::Update(float timeDelta)
{
	UpdatePlayerInput();
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{
		if(m_actors[actorIndex] != nullptr)
			m_actors[actorIndex]->Update(timeDelta);
	}
	for(size_t projectileIndex = 0; projectileIndex < m_projectiles.size(); projectileIndex++)
	{
		if(m_projectiles[projectileIndex] != nullptr)
			m_projectiles[projectileIndex]->Update(timeDelta);
	}
	for(size_t portalIndex = 0; portalIndex < m_portals.size(); portalIndex++)
	{
		if(m_portals[portalIndex] != nullptr)
			m_portals[portalIndex]->Update(timeDelta);
	}
	for(size_t itemIndex = 0; itemIndex < m_items.size(); itemIndex++)
	{
		if(m_items[itemIndex] != nullptr)
			m_items[itemIndex]->Update(timeDelta);
	}
	ActorToTileCollision();
	ProjectileToActorCollision();
	ActorToActorAttackCollision();
	ActorToActorCollision();
	ActorToPortalCollision();
	ActorToItemCollision();
	DeleteDeadEntities();
}

void Map::Render()
{
	Vector2 bottomLeftOrtho = Vector2(0.f, 0.f);
	Vector2 topRightOrtho = Vector2(g_maxNumTilesViewHorizontally, g_maxNumTilesViewVertically);

	if(!m_isFullMapView)
	{
		int actorIndex = GetActorIndex();
		
		if(actorIndex != -1)
		{
			m_camera->m_worldPos = m_actors[actorIndex]->m_center; //NOTE: I feel okay updating the camera position here even if we are in the render function because camera is specific to rendering and setting ortho. Don't kill me, Squirrel.
		}
		bottomLeftOrtho = Vector2(m_camera->m_worldPos.x - (m_camera->m_numTilesViewHorizontally * .5f), m_camera->m_worldPos.y - (m_camera->m_numTilesViewVertically * .5f));
		topRightOrtho = Vector2(m_camera->m_worldPos.x + (m_camera->m_numTilesViewHorizontally * .5f), m_camera->m_worldPos.y + (m_camera->m_numTilesViewVertically * .5f));	

		if(bottomLeftOrtho.x < 0.f)
		{
			topRightOrtho.x += (bottomLeftOrtho.x * -1.f);
			bottomLeftOrtho.x = 0.0f;
		}
		if(bottomLeftOrtho.y < 0.f)
		{
			topRightOrtho.y += (bottomLeftOrtho.y * -1.f);
			bottomLeftOrtho.y = 0.0f;
		}
		if(topRightOrtho.x > (float)m_dimensions.x)
		{
			float cameraAdjustmentX = topRightOrtho.x - (float)m_dimensions.x;

			bottomLeftOrtho.x -= cameraAdjustmentX;
			topRightOrtho.x = (float)m_dimensions.x;		
		}
		if(topRightOrtho.y > (float)m_dimensions.y)
		{
			float cameraAdjustmentY = topRightOrtho.y - (float)m_dimensions.y;

			bottomLeftOrtho.y -= cameraAdjustmentY;
			topRightOrtho.y = (float)m_dimensions.y;
		}
	}		

	g_theRenderer->SetOrtho(bottomLeftOrtho, topRightOrtho);
	g_theRenderer->ClearColor(g_backgroundColor);
	g_theRenderer->Clear();

	for(size_t i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i].Render();
	}

	std::vector<Entity*> drawEntities;

	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{	
		if(m_actors[actorIndex] != nullptr)
		{
			drawEntities.push_back(m_actors[actorIndex]);			
		}		
	}
	for(size_t projectileIndex = 0; projectileIndex < m_projectiles.size(); projectileIndex++)
	{
		if(m_projectiles[projectileIndex] != nullptr)
		{
			drawEntities.push_back(m_projectiles[projectileIndex]);
		}		
	}
	for(size_t portalIndex = 0; portalIndex < m_portals.size(); portalIndex++)
	{
		if(m_portals[portalIndex] != nullptr)
		{
			drawEntities.push_back(m_portals[portalIndex]);
		}
	}
	for(size_t itemIndex = 0; itemIndex < m_items.size(); itemIndex++)
	{
		if(m_items[itemIndex] != nullptr)
		{
			drawEntities.push_back(m_items[itemIndex]);			
		}
	}

	//bubble sort through entities
	for (size_t i = 0; i < drawEntities.size()-1; i++)
	{
		// Last i elements are already in place   
		for (size_t j = 0; j < drawEntities.size()-i-1; j++) 
		{
			if (drawEntities[j]->m_center.y < drawEntities[j+1]->m_center.y)
			{					
				Entity* temp = drawEntities[j];
				drawEntities[j] = drawEntities[j+1];
				drawEntities[j+1] = temp;					
			}
		}
	}

	for(size_t i = 0; i < drawEntities.size(); i++)
	{
		drawEntities[i]->Render();
	}	
}

void Map::SpawnActorByName(std::string actorType, Vector2 spawnPosition)
{
	ActorDefinition* spawnDef = ActorDefinition::s_actorDefinitions[actorType];

	SpawnActorByDefinition(spawnDef, spawnPosition);
}


void Map::SpawnItemByName(std::string itemName, Vector2 spawnPosition)
{
	ItemDefinition* spawnDef = ItemDefinition::s_itemDefinitions[itemName];
	
	SpawnItemByDefinition(spawnDef, spawnPosition);
}

void Map::SpawnActorByDefinition(ActorDefinition* actorDefinition, Vector2 spawnPosition)
{
	Actor* newActor = new Actor(actorDefinition);
	newActor->m_center = spawnPosition;
	m_actors.push_back(newActor);
}

void Map::SpawnItemByDefinition(ItemDefinition* itemDefinition, Vector2 spawnPosition)
{
	Item* newItem = new Item(itemDefinition);
	newItem->m_center = spawnPosition;
	m_items.push_back(newItem);
}

void Map::SpawnProjectileFromActor(Actor* spawningActor, std::string projectileType)
{
	ProjectileDefinition* spawnDef = ProjectileDefinition::s_projectileDefinitions[projectileType];

	Projectile* newProjectile = new Projectile(spawnDef);
	newProjectile->m_center = spawningActor->m_center;

	if(spawningActor->m_shootDirection == Vector2(0,0))
	{
		newProjectile->m_movementVector = spawningActor->m_movementVector;

		if(newProjectile->m_movementVector == Vector2(0,0))
		{
			newProjectile->m_movementVector = g_southVector2;
		}
	}
	else
	{
		newProjectile->m_movementVector = spawningActor->m_shootDirection;
	}
	

	newProjectile->m_attackableFactions = spawningActor->m_actorDef->m_attackableFactions;
	m_projectiles.push_back(newProjectile);
}

void Map::SpawnPortal(Portal* spawnPortal)
{
	bool didSpawnPortal = false;
	std::vector<Tile*> validSpawnTiles;
	for(size_t tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++)
	{
		if(m_tiles[tileIndex].m_tileDef->m_Name == spawnPortal->m_onTileType)
		{
			validSpawnTiles.push_back(&m_tiles[tileIndex]);
		}
	}

	if((int)validSpawnTiles.size() > 0)
	{
		int spawnTileIndex = GetRandomIntInRange(0, (int)validSpawnTiles.size() - 1);
		spawnPortal->m_center = Vector2(((float)validSpawnTiles[spawnTileIndex]->m_tileCoords.x) + 0.5f, ((float)validSpawnTiles[spawnTileIndex]->m_tileCoords.y) + 0.5f);
		spawnPortal->m_myMap = this;
		didSpawnPortal = true;
	}

	if(didSpawnPortal == false)
	{
		//TODO: Write code here to change an existing tile that the player can get to somewhere in the map to the correct type that you want and put the tile there
	}
	m_portals.push_back(spawnPortal);
}

void Map::ActorToTileCollision()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{
		if(m_actors[actorIndex] != nullptr)
		{		
			Actor*& entityToTest = m_actors[actorIndex];

			IntVector2 tileOfEntityCenter = ConvertVector2ToIntVector2(entityToTest->m_center);

			IntVector2 eastTile = IntVector2(tileOfEntityCenter.x + 1, tileOfEntityCenter.y);
			IntVector2 westTile = IntVector2(tileOfEntityCenter.x - 1 , tileOfEntityCenter.y);
			IntVector2 northTile = IntVector2(tileOfEntityCenter.x, tileOfEntityCenter.y + 1);
			IntVector2 southTile = IntVector2(tileOfEntityCenter.x, tileOfEntityCenter.y - 1);
			IntVector2 northEastTile = IntVector2(tileOfEntityCenter.x + 1, tileOfEntityCenter.y + 1);
			IntVector2 northWestTile = IntVector2(tileOfEntityCenter.x - 1, tileOfEntityCenter.y + 1);
			IntVector2 southEastTile = IntVector2(tileOfEntityCenter.x + 1, tileOfEntityCenter.y - 1);
			IntVector2 southWestTile = IntVector2(tileOfEntityCenter.x - 1, tileOfEntityCenter.y - 1);

			DetectActorToTileCollision(actorIndex, entityToTest->m_center, northEastTile, northeast); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, northWestTile, northwest); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, southEastTile, southeast); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, southWestTile, southwest); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, eastTile, east); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, westTile, west); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, northTile, north); 
			DetectActorToTileCollision(actorIndex, entityToTest->m_center, southTile, south);				
		}			
	}
}

void Map::DetectActorToTileCollision(int entityIndex, Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection)
{
	if(tileCoordinate.x >= 0 && tileCoordinate.y >= 0)
	{
		int tileIndex =  (m_dimensions.y * tileCoordinate.x) + tileCoordinate.y;

		IntVector2 coords = m_tiles[tileIndex].m_tileCoords;
		if((!m_tiles[tileIndex].m_tileDef->m_allowsFlying && m_actors[entityIndex]->m_entityDef->m_doesFly) || (!m_tiles[tileIndex].m_tileDef->m_allowsWalking && m_actors[entityIndex]->m_entityDef->m_doesWalk))
		{
			Vector2 collisionPoint;
			switch(tileDirection)
			{
			case east:
				collisionPoint = Vector2((float)tileCoordinate.x, entityCenter.y);
				break;
			case west:
				collisionPoint = Vector2((float)tileCoordinate.x + 1, entityCenter.y);
				break;
			case north:
				collisionPoint = Vector2(entityCenter.x, (float)tileCoordinate.y);
				break;
			case south:
				collisionPoint = Vector2(entityCenter.x, (float)tileCoordinate.y + 1.f);
				break;
			case northeast:
				collisionPoint = Vector2((float)tileCoordinate.x, (float)tileCoordinate.y);
				break;
			case northwest:
				collisionPoint = Vector2((float)tileCoordinate.x + 1.f, (float)tileCoordinate.y);
				break;
			case southeast:
				collisionPoint = Vector2((float)tileCoordinate.x, (float)tileCoordinate.y + 1.f);
				break;
			case southwest:
				collisionPoint = Vector2((float)tileCoordinate.x + 1.f, (float)tileCoordinate.y + 1.f);
				break;
			}

			bool isColliding = Disc2(m_actors[entityIndex]->m_center, m_actors[entityIndex]->m_entityDef->m_physicsRadius).IsPointInside(collisionPoint);
			if(isColliding)
			{
				float displacement = GetDistance(m_actors[entityIndex]->m_center, collisionPoint);

				float amountToPush =  m_actors[entityIndex]->m_entityDef->m_physicsRadius - displacement;
				Vector2 pushOutDirection = ( m_actors[entityIndex]->m_center - collisionPoint).GetNormalized();

				m_actors[entityIndex]->m_center += pushOutDirection * (amountToPush);
			}
		}
	}	
}

void Map::ProjectileToActorCollision()
{
	for(size_t projectileIndex = 0; projectileIndex < m_projectiles.size(); projectileIndex++)
	{
		if(m_projectiles[projectileIndex] != nullptr)
		{		
			for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
			{
				if(m_actors[actorIndex] != nullptr)
				{
					bool isValidTarget = false;
					for(size_t factionIndex = 0; factionIndex < m_projectiles[projectileIndex]->m_attackableFactions.size(); factionIndex++)
					{
						if(m_projectiles[projectileIndex]->m_attackableFactions[factionIndex] == m_actors[actorIndex]->m_actorDef->m_faction)
						{
							isValidTarget = true;
							break;
						}
					}
					if(isValidTarget)
					{
						bool doesOverlap = DoDiscsOverlap(Disc2(m_projectiles[projectileIndex]->m_center, m_projectiles[projectileIndex]->m_entityDef->m_physicsRadius), Disc2(m_actors[actorIndex]->m_center, m_actors[actorIndex]->m_entityDef->m_physicsRadius));

						if(doesOverlap)
						{
							m_projectiles[projectileIndex]->m_health--;
							m_actors[actorIndex]->m_health -= m_projectiles[projectileIndex]->m_damage;
							m_actors[actorIndex]->m_wasJustHit = true;
						}
					}
				}				
			}
		}
	}
}

//TODO: For now, just teleport the player.  Might have other actors be able to follow players through maps later.
void Map::ActorToPortalCollision()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{		
		bool doesCollide = false;
		if(m_actors[actorIndex] != nullptr)
		{
			if(!m_actors[actorIndex]->m_isDead)
			{				
				for(size_t portalIndex = 0; portalIndex < m_portals.size(); portalIndex++)
				{
					doesCollide =  Disc2(m_actors[actorIndex]->m_center, m_actors[actorIndex]->m_entityDef->m_physicsRadius).IsPointInside(m_portals[portalIndex]->m_center);

					if(doesCollide)
					{
						if(m_actors[actorIndex]->m_entityDef->m_name == "Player" && !m_portals[portalIndex]->m_isPlayerCollisionDisabled)
						{
							Actor*& player = m_actors[actorIndex];
							m_actors[actorIndex];
							m_portals[portalIndex]->m_destinationPortal->m_isPlayerCollisionDisabled = true;

							m_portals[portalIndex]->m_destinationMap->m_actors.push_back(player);

							player->m_center = m_portals[portalIndex]->m_destinationCoordinates;

							m_actors[actorIndex] = nullptr;
							m_actors.erase(m_actors.begin() + actorIndex);
							

							m_portals[portalIndex]->m_destinationPortal->m_isPlayerCollisionDisabled = true;

							g_mapChangeTarget = m_portals[portalIndex]->m_destinationMapName;  //set the target for changing to at the end of the frame.
							break;
						}
						else
						{
							float displacement = GetDistance(m_actors[actorIndex]->m_center, m_portals[portalIndex]->m_center);
							float amountToPush = displacement - (m_actors[actorIndex]->m_entityDef->m_physicsRadius + m_portals[portalIndex]->m_entityDef->m_physicsRadius);

							amountToPush = fabsf(amountToPush);

							Vector2 pushOutDirection = (m_actors[actorIndex]->m_center - m_portals[portalIndex]->m_center).GetNormalized();

							m_actors[actorIndex]->m_center += pushOutDirection * (amountToPush);
						}
					}	
					else if(m_actors[actorIndex]->m_entityDef->m_name == "Player")
					{
						m_portals[portalIndex]->m_isPlayerCollisionDisabled = false;
					}
				}				
			}
		}
	}
};

void Map::ActorToActorCollision()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{		
		bool doesCollide = false;
		if(m_actors[actorIndex] != nullptr)
		{
			for(size_t collisionActorIndex = 0; collisionActorIndex < m_actors.size(); collisionActorIndex++)
			{
				if(collisionActorIndex != actorIndex)
				{
					if(m_actors[collisionActorIndex] != nullptr)
					{
						if(m_actors[collisionActorIndex]->m_actorDef->m_doesHaveActorToActorPhysics)
						{
							Disc2 actorDisc = Disc2(m_actors[actorIndex]->m_center, m_actors[actorIndex]->m_entityDef->m_physicsRadius);
							Disc2 collisionActorDisc = Disc2(m_actors[collisionActorIndex]->m_center, m_actors[collisionActorIndex]->m_entityDef->m_physicsRadius);

							doesCollide = DoDiscsOverlap(actorDisc, collisionActorDisc); 
							if(doesCollide)
							{
								float displacement = GetDistance(m_actors[actorIndex]->m_center, m_actors[collisionActorIndex]->m_center);
								float totalAmountToPush = (m_actors[actorIndex]->m_entityDef->m_physicsRadius + m_actors[collisionActorIndex]->m_entityDef->m_physicsRadius) - displacement;

								totalAmountToPush = fabsf(totalAmountToPush);
							

								float individualAmountToPushActor = 0.f;
								float individualAmountToPushCollisionActor = 0.f;

								//TODO: mass calculator.  Currently some issues with too much mass applied
								float massRatioActor = m_actors[actorIndex]->m_actorDef->m_mass/(m_actors[actorIndex]->m_actorDef->m_mass + m_actors[collisionActorIndex]->m_actorDef->m_mass);
								float massRatioCollisionActor = m_actors[collisionActorIndex]->m_actorDef->m_mass/(m_actors[actorIndex]->m_actorDef->m_mass + m_actors[collisionActorIndex]->m_actorDef->m_mass);

								individualAmountToPushActor = totalAmountToPush * massRatioCollisionActor;
								individualAmountToPushCollisionActor = totalAmountToPush * massRatioActor;
								

								Vector2 pushOutDirectionActor = (m_actors[actorIndex]->m_center - m_actors[collisionActorIndex]->m_center).GetNormalized();
								m_actors[actorIndex]->m_center += pushOutDirectionActor * (individualAmountToPushActor);

								Vector2 pushOutDirectionCollisionActor = (m_actors[collisionActorIndex]->m_center - m_actors[actorIndex]->m_center).GetNormalized();
								m_actors[actorIndex]->m_center += pushOutDirectionCollisionActor * (individualAmountToPushCollisionActor);
							}
						}						
					}					
				}
			}
		}
	}	
}

/*
TODO: Could use a tag like this (canPickupItems) to do something cool where one of your missions is to race an NPC around the map to collect items.
Pick a random item in the map and generate a dijkstra map there. THEN if it's picked up by the player OR if the NPC themselves pick it up,
rinse and repeat until there are no more things in the map.  Keep a counter of how much stuff each of them have.  Could be cool! :P
*/
void Map::ActorToItemCollision()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{
		if(m_actors[actorIndex] != nullptr)
		{
			if(m_actors[actorIndex]->m_tags.HasTags("canPickupItems")) 
			{			
				bool doesCollide = false;
				if(m_actors[actorIndex] != nullptr)
				{
					if(m_actors[actorIndex]->m_isActorTryingToInteract)
					{
						//go ahead and reset the actor's interact flag and then proceed to pickup any items the actor collides with
						m_actors[actorIndex]->m_isActorTryingToInteract = false;

						for(size_t itemCollisionIndex = 0; itemCollisionIndex < m_items.size(); itemCollisionIndex++)
						{
							if(m_items[itemCollisionIndex] != nullptr)
							{
								Disc2 itemDisc = Disc2(m_items[itemCollisionIndex]->m_center, m_items[itemCollisionIndex]->m_entityDef->m_physicsRadius);
								Disc2 collisionActorDisc = Disc2(m_actors[actorIndex]->m_center, m_actors[actorIndex]->m_entityDef->m_physicsRadius);

								doesCollide = DoDiscsOverlap(itemDisc, collisionActorDisc); 
								if(doesCollide)
								{
									int itemSlot = (int)m_items[itemCollisionIndex]->m_itemDef->m_slot;
									if(itemSlot != NONE_SLOT)
									{
										//drop item currently occupying the slot.  TODO: maybe want to handle this differently if you had a full inventory of items

										if(m_actors[actorIndex]->m_equippedItems[itemSlot] != nullptr)
										{
											m_actors[actorIndex]->m_equippedItems[itemSlot]->m_isEquipped = false;
											m_actors[actorIndex]->m_equippedItems[itemSlot]->m_center = m_actors[actorIndex]->m_center;

											m_items.push_back(m_actors[actorIndex]->m_equippedItems[itemSlot]);
											m_actors[actorIndex]->m_equippedItems[itemSlot] = nullptr;
										}					

										m_items[itemCollisionIndex]->m_isEquipped = true;
										m_actors[actorIndex]->m_equippedItems[itemSlot] = m_items[itemCollisionIndex];

										m_items[itemCollisionIndex] = nullptr;
										m_items.erase(m_items.begin() + itemCollisionIndex);
									}
									else
									{
										m_actors[actorIndex]->m_inventory.push_back(m_items[itemCollisionIndex]);

										m_items[itemCollisionIndex] = nullptr;
										m_items.erase(m_items.begin() + itemCollisionIndex);
									}

								}
							}				
						}
					}				
				}
			}	
		}			
	}
}

void Map::ActorToActorAttackCollision()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{		
		bool doesCollide = false;
		if(m_actors[actorIndex] != nullptr)
		{
			if(m_actors[actorIndex]->m_isAttacking)
			{
				m_actors[actorIndex]->m_isAttacking = false;
				for(size_t collisionActorIndex = 0; collisionActorIndex < m_actors.size(); collisionActorIndex++)
				{
					if(collisionActorIndex != actorIndex)
					{
						if(m_actors[collisionActorIndex] != nullptr)
						{
							if(m_actors[collisionActorIndex]->m_actorDef->m_doesHaveActorToActorPhysics)
							{
								bool isValidTarget = false;
								for(size_t factionIndex = 0; factionIndex < m_actors[actorIndex]->m_actorDef->m_attackableFactions.size(); factionIndex++)
								{
									if( m_actors[actorIndex]->m_actorDef->m_attackableFactions[factionIndex] == m_actors[collisionActorIndex]->m_actorDef->m_faction)
									{
										isValidTarget = true;
										break;
									}
								}
								if(isValidTarget)
								{
									Disc2 actorDisc = Disc2(m_actors[actorIndex]->m_center, (float)m_actors[actorIndex]->m_stats.stats[STAT_MELEE_RANGE]);
									Disc2 collisionActorDisc = Disc2(m_actors[collisionActorIndex]->m_center, m_actors[collisionActorIndex]->m_entityDef->m_physicsRadius);

									doesCollide = DoDiscsOverlap(actorDisc, collisionActorDisc); 

									if(doesCollide)
									{
										//calculate stats and final damage
										int armorStat = m_actors[collisionActorIndex]->m_stats.stats[STAT_ARMOR];
										for(size_t equippedItemIndex = 0; equippedItemIndex < m_actors[actorIndex]->m_equippedItems.size(); equippedItemIndex++)
										{
											if(m_actors[collisionActorIndex]->m_equippedItems[equippedItemIndex] != nullptr)
											{
												armorStat += m_actors[collisionActorIndex]->m_equippedItems[equippedItemIndex]->m_itemDef->m_stats.stats[STAT_ARMOR];
											}
										}

										float damageResistancePercentage = 0.f;
										if(armorStat != 0)
										{
											damageResistancePercentage = (float)armorStat/(float)g_maxArmorStat; //resistance bonus on taking damage
										}

										int strengthStat = m_actors[actorIndex]->m_stats.stats[STAT_STRENGTH];
										for(size_t equippedItemIndex = 0; equippedItemIndex < m_actors[actorIndex]->m_equippedItems.size(); equippedItemIndex++)
										{
											if(m_actors[actorIndex]->m_equippedItems[equippedItemIndex] != nullptr)
											{
												strengthStat += m_actors[actorIndex]->m_equippedItems[equippedItemIndex]->m_itemDef->m_stats.stats[STAT_STRENGTH];
											}
										}
										float strengthIncreasePercentage = 0.f;
										if(armorStat != 0)
										{
											strengthIncreasePercentage = (float)strengthStat/(float)g_maxStrengthStat; //damage boonus on attack
										}

										float damageStat = (float)m_actors[actorIndex]->m_stats.stats[STAT_DAMAGE];
										for(size_t equippedItemIndex = 0; equippedItemIndex < m_actors[actorIndex]->m_equippedItems.size(); equippedItemIndex++)
										{
											if(m_actors[actorIndex]->m_equippedItems[equippedItemIndex] != nullptr)
											{
												damageStat += m_actors[actorIndex]->m_equippedItems[equippedItemIndex]->m_itemDef->m_stats.stats[STAT_DAMAGE];
											}
										}

										float totalDamage = (damageStat + ((float)damageStat * strengthIncreasePercentage)) -  ((float)damageStat * damageResistancePercentage);

										m_actors[collisionActorIndex]->m_health -= totalDamage;
										m_actors[collisionActorIndex]->m_wasJustHit = true;
									}
								}
								}
							
						}
					}
				}
			}			
		}
	}
}



void Map::DeleteDeadEntities()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{		
		if(m_actors[actorIndex] != nullptr)
		{
			if(m_actors[actorIndex]->m_isDead)
			{
				delete m_actors[actorIndex];
				m_actors[actorIndex] = nullptr;
			}
		}
	}
	for(size_t projectileIndex = 0; projectileIndex < m_projectiles.size(); projectileIndex++)
	{		
		if(m_projectiles[projectileIndex] != nullptr)
		{
			if(m_projectiles[projectileIndex]->m_isDead)
			{
				delete m_projectiles[projectileIndex];
				m_projectiles[projectileIndex] = nullptr;
			}
		}
	}
}

void Map::UpdatePlayerInput()
{
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_M))
	{
		if(m_isFullMapView == true)
		{
			m_isFullMapView = false;
		}
		else
		{
			m_isFullMapView = true;
		}
	}
}

int Map::GetActorIndex()
{
	for(size_t actorIndex = 0; actorIndex < m_actors.size(); actorIndex++)
	{
		if(m_actors[actorIndex] != nullptr)
		{
			if(m_actors[actorIndex]->m_entityDef->m_name == "Player")
			{
				return actorIndex;
			}
		}		
	}
	
	return -1; //we failed
}