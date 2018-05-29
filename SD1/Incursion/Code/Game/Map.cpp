#include "Map.hpp"
#include <vector>
#include "Engine/Math/MathUtils.hpp"
#include "Entity.hpp"
#include "PlayerTank.hpp"
#include "EnemyTank.hpp"
#include "GameCommon.hpp"
#include "Camera.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Bullet.hpp"
#include "BossTank.hpp"

Camera* camera = new Camera();

void Map::Update(float timeDelta)
{
	for(size_t i = 0; i < m_enemyTurretEntities.size(); i++)
	{		
		if(m_enemyTurretEntities[i] != nullptr)
		{ 
			if(!m_enemyTurretEntities[i]->m_isDead)
			{				
				RaycastResult result = Raycast(m_enemyTurretEntities[i]->m_center, m_enemyTurretEntities[i]->m_turretOrientationVector, g_turretLaserDistance);
				if(result.didCollide)
				{
					m_enemyTurretEntities[i]->m_turretLaserDrawDistance = GetDistance(m_enemyTurretEntities[i]->m_center, result.collisionPosition);
				}
				else
				{
					m_enemyTurretEntities[i]->m_turretLaserDrawDistance = g_turretLaserDistance;
				}
			}
			if(!m_enemyTurretEntities[i]->m_isDead && !m_playerTankEntities[0]->m_isDead)
			{
				float turretToPlayerDistance = GetDistance(m_enemyTurretEntities[i]->m_center, m_playerTankEntities[0]->m_center);
				if(turretToPlayerDistance <= g_maxTurretToPlayerDistance)
				{
					m_enemyTurretEntities[i]->m_doesHaveTargetLineOfSight  = HasLineOfSight(m_enemyTurretEntities[i]->m_center, m_playerTankEntities[0]->m_center);
					if(m_enemyTurretEntities[i]->m_doesHaveTargetLineOfSight)
					{
						Vector2 orientationToTargetVector = (m_playerTankEntities[0]->m_center - m_enemyTurretEntities[i]->m_center ).GetNormalized();
						m_enemyTurretEntities[i]->m_targetDegrees = orientationToTargetVector.GetOrientationDegrees();

						float dotProductResult = DotProduct((m_playerTankEntities[0]->m_center - m_enemyTurretEntities[i]->m_center).GetNormalized(),  m_enemyTurretEntities[i]->m_turretOrientationVector);
						if(dotProductResult > CosDegrees(5.f))
						{
							if(m_enemyTurretEntities[i]->m_isReadyToShoot)
							{
								SoundID sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
								g_theAudio->PlaySound( sound );
								InstantiateEnemyBullet(m_enemyTurretEntities[i]->m_turretRotationDegrees, m_enemyTurretEntities[i]->m_center);
								m_enemyTurretEntities[i]->m_isReadyToShoot = false;
							}
						}
					}
				}	
			}
		}
	}

	for(size_t i = 0; i < m_enemyTankEntities.size(); i++)
	{		
		if(m_enemyTankEntities[i] != nullptr)
		{ 
			if(!m_enemyTankEntities[i]->m_isDead && !m_playerTankEntities[0]->m_isDead)
			{				
				m_enemyTankEntities[i]->m_doesHaveTargetLineOfSight  = HasLineOfSight(m_enemyTankEntities[i]->m_center, m_playerTankEntities[0]->m_center);
				if(m_enemyTankEntities[i]->m_doesHaveTargetLineOfSight)
				{
					Vector2 orientationToTargetVector = (m_playerTankEntities[0]->m_center - m_enemyTankEntities[i]->m_center ).GetNormalized();
					m_enemyTankEntities[i]->m_targetDegrees = orientationToTargetVector.GetOrientationDegrees();	
					m_enemyTankEntities[i]->m_targetPosition = m_playerTankEntities[0]->m_center;

					float dotProductResult = DotProduct((m_playerTankEntities[0]->m_center - m_enemyTankEntities[i]->m_center).GetNormalized(),  m_enemyTankEntities[i]->m_movementVector.GetNormalized());
					if(dotProductResult > CosDegrees(5.f))
					{
						if(m_enemyTankEntities[i]->m_isReadyToShoot)
						{
							SoundID sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
							g_theAudio->PlaySound( sound );
							InstantiateEnemyBullet(m_enemyTankEntities[i]->m_rotationDegrees, m_enemyTankEntities[i]->m_center);
							m_enemyTankEntities[i]->m_isReadyToShoot = false;
						}
					}
					if(dotProductResult > CosDegrees(45.f))
					{						
						m_enemyTankEntities[i]->m_isChargingPlayer;
					}
				}
										
				else
				{
					//roam around
				}				
			}
		}
	}	

	for(size_t i = 0; i < m_bossEntities.size(); i++)
	{		
		if(m_bossEntities[i] != nullptr)
		{ 
			if(!m_bossEntities[i]->m_isDead && !m_playerTankEntities[0]->m_isDead)
			{				
				m_bossEntities[i]->m_doesHaveTargetLineOfSight  = HasLineOfSight(m_bossEntities[i]->m_center, m_playerTankEntities[0]->m_center);
				if(m_bossEntities[i]->m_doesHaveTargetLineOfSight)
				{
					Vector2 orientationToTargetVector = (m_playerTankEntities[0]->m_center - m_bossEntities[i]->m_center ).GetNormalized();
					m_bossEntities[i]->m_targetDegrees = orientationToTargetVector.GetOrientationDegrees();	
					m_bossEntities[i]->m_targetPosition = m_playerTankEntities[0]->m_center;

					float dotProductResult = DotProduct((m_playerTankEntities[0]->m_center - m_bossEntities[i]->m_center).GetNormalized(),  m_bossEntities[i]->m_orientationVector.GetNormalized());
					if(dotProductResult > CosDegrees(20.f))
					{
						if(m_bossEntities[i]->m_isReadyToShoot && m_bossEntities[i]->m_attackMode == normalFire)
						{
							SoundID sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
							g_theAudio->PlaySound( sound );
							InstantiateEnemyBullet(m_bossEntities[i]->m_rotationDegrees, m_bossEntities[i]->m_center);
							m_bossEntities[i]->m_shootCount++; 
							m_bossEntities[i]->m_isReadyToShoot = false;
						}						
					}				
				}
				if(m_bossEntities[i]->m_isReadyToShoot && m_bossEntities[i]->m_attackMode == airStrike)
				{
					SoundID sound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
					g_theAudio->PlaySound( sound );
					int randomXCoordinate = GetRandomIntInRange(1, 19);
					int randomYCoordinate = GetRandomIntInRange(1, 19);
					InstantiateAirStrike(Vector2((float)randomXCoordinate + 0.5f, (float)randomYCoordinate + 0.5f), 2.f, 3.f);
					m_bossEntities[i]->m_isReadyToShoot = false;
				}
			
			}
		}
	}

	//update player tanks
	for(size_t i = 0; i < m_playerTankEntities.size(); i++)
	{		
		if(g_isPlayerShooting && m_playerTankEntities[i]->m_isReadyToShoot && !m_playerTankEntities[i]->m_isDead)
		{
			InstantiatePlayerBullet();
			SoundID sound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
			g_theAudio->PlaySound( sound );

			m_playerTankEntities[i]->m_isReadyToShoot = false;
		}
		m_playerTankEntities[i]->Update(timeDelta);		
	}

	//update enemy turrets
	for(size_t i = 0; i < m_enemyTurretEntities.size(); i++)
	{		
		if(m_enemyTurretEntities[i] != nullptr)
		{
			m_enemyTurretEntities[i]->Update(timeDelta);	
		}
	}

	//update enemy tanks
	for(size_t i = 0; i < m_enemyTankEntities.size(); i++)
	{		
		if(m_enemyTankEntities[i] != nullptr)
		{
			m_enemyTankEntities[i]->Update(timeDelta);
		}
	}

	for(size_t i = 0; i < m_bossEntities.size(); i++)
	{		
		if(m_bossEntities[i] != nullptr)
		{
			m_bossEntities[i]->Update(timeDelta);
		}
	}

	//update bullets
	for(size_t i = 0; i < m_bulletEntities.size(); i++)
	{		
		if(m_bulletEntities[i] != nullptr)
		{
			BulletToTileCollision(m_bulletEntities[i], timeDelta);
		}
	}
	for(size_t i = 0; i < m_airstrikes.size(); i++)
	{		
		if(m_airstrikes[i] != nullptr)
		{
			m_airstrikes[i]->Update(timeDelta);
			if(m_airstrikes[i]->m_isExploding == true)
			{
				InstantiateExplosion(Vector2(m_airstrikes[i]->m_position.x, m_airstrikes[i]->m_position.y + .2f), 1.f, 2.f);
				InstantiateExplosion(Vector2(m_airstrikes[i]->m_position.x - .2f, m_airstrikes[i]->m_position.y), .5f, 1.5f);
				InstantiateExplosion(Vector2(m_airstrikes[i]->m_position.x + .3f, m_airstrikes[i]->m_position.y), .3f, 1.f);
				InstantiateExplosion(Vector2(m_airstrikes[i]->m_position.x + .2f, m_airstrikes[i]->m_position.y - .2f), .2f, .5f);
			}			
		}
	}
	for(size_t i = 0; i < m_explosions.size(); i++)
	{
		if(m_explosions[i] != nullptr)
		{
			m_explosions[i]->Update(timeDelta);
		}
	}

	BulletToEntityCollision();
	PlayerTankToAirStrikeCollisions();
	EntityToEntityCollision();
	PlayerTankToTileCollision();
	EnemyTankToTileCollision();

	DeleteDeadEntities();
}

void Map::Render()
{
	//set up screen
	camera->m_worldPos = m_playerTankEntities[0]->m_center;
	camera->m_orientationDegrees = 0;
	camera->m_numTilesViewVertically = 12;
	camera->m_numTilesViewHorizontally = camera->m_numTilesViewVertically / (4.f/3.f);
	Vector2 bottomLeftOrtho = Vector2(camera->m_worldPos.x - (camera->m_numTilesViewHorizontally * .5f), camera->m_worldPos.y - (camera->m_numTilesViewVertically * .5f));
	Vector2 topRightOrtho = Vector2(camera->m_worldPos.x + (camera->m_numTilesViewHorizontally * .5f), camera->m_worldPos.y + (camera->m_numTilesViewVertically * .5f));	

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
	if(topRightOrtho.x > m_numTilesX)
	{
		float cameraAdjustmentX = topRightOrtho.x - m_numTilesX;

		bottomLeftOrtho.x -= cameraAdjustmentX;
		topRightOrtho.x = (float)m_numTilesX;		
	}
	if(topRightOrtho.y > m_numTilesY)
	{
		float cameraAdjustmentY = topRightOrtho.y - m_numTilesY;

		bottomLeftOrtho.y -= cameraAdjustmentY;
		topRightOrtho.y = (float)m_numTilesY;

	}
	g_theRenderer->SetOrtho(bottomLeftOrtho, topRightOrtho);
	g_theRenderer->ClearColor(g_backgroundColor);
	g_theRenderer->Clear();

	for(size_t i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i].Render();
	}
	for(size_t i = 0; i < m_playerTankEntities.size(); i++)
	{
		m_playerTankEntities[i]->Render();
	}
	for(size_t i = 0; i < m_enemyTurretEntities.size(); i++)
	{
		if(m_enemyTurretEntities[i] != nullptr)
		{
			m_enemyTurretEntities[i]->Render();
		}
	}
	for(size_t i = 0; i < m_enemyTankEntities.size(); i++)
	{
		if(m_enemyTankEntities[i] != nullptr)
		{
			m_enemyTankEntities[i]->Render();
		}
	}
	for(size_t i = 0; i < m_bulletEntities.size(); i++)
	{
		if(m_bulletEntities[i] != nullptr)
		{
			m_bulletEntities[i]->Render();
		}
	}
	for(size_t i = 0; i < m_bossEntities.size(); i++)
	{
		if(m_bossEntities[i] != nullptr)
		{
			m_bossEntities[i]->Render();
		}
	}

	if(m_playerTankEntities[0]->m_isDead)
	{
		g_isPausedDisabled = true;
		if(m_playerTankEntities[0]->m_age > m_playerTankEntities[0]->m_deathTime + 2.0f)
		{
			g_gameState = respawnpaused;
			g_isPausedDisabled = false;
		}
	}
	for(size_t i = 0; i < m_airstrikes.size(); i++)
	{
		if(m_airstrikes[i] != nullptr)
		{
			m_airstrikes[i]->Render();
		}
	}
	for(size_t i = 0; i < m_explosions.size(); i++)
	{
		if(m_explosions[i] != nullptr)
		{
			m_explosions[i]->Render();
		}
	}
	if(g_gameState == paused)
	{
		g_theRenderer->DrawAABB(AABB2(bottomLeftOrtho, topRightOrtho), Rgba(0.f, 0.f, 0.f, .50f));
		std::string pauseTexture;
		
		pauseTexture = "Data/Images/PauseScreenTransparent.png";
		
		Texture* pauseScreenTexture = g_theRenderer->CreateOrGetTexture(pauseTexture);
		g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *pauseScreenTexture, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
	}
	if(g_gameState == respawnpaused)
	{
		g_theRenderer->DrawAABB(AABB2(bottomLeftOrtho, topRightOrtho), Rgba(0.f, 0.f, 0.f, .50f));
		std::string pauseTexture;

		pauseTexture = "Data/Images/DeadRespawnTransparent.png";

		Texture* pauseScreenTexture = g_theRenderer->CreateOrGetTexture(pauseTexture);
		g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *pauseScreenTexture, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
	}
}

void Map::GenerateMap(int numTilesX, int numTilesY)
{
	for(int xCoordinate = 0; xCoordinate < numTilesX; xCoordinate++)
	{	
		for(int yCoordinate = 0; yCoordinate < numTilesY; yCoordinate++)
		{
			Tile newTile;
			
			newTile.m_tileCoords = IntVector2(xCoordinate, yCoordinate);
			newTile.m_tileBounds = AABB2(Vector2((float)xCoordinate, (float)yCoordinate),Vector2((float)xCoordinate + 1.f, (float)yCoordinate + 1.f)); //tiles are 1 across
			int randomPercentage = GetRandomIntInRange(1, 10); //TODO:Tiles have a 10% chance of being stone for now

			if((xCoordinate == 1 && yCoordinate == 1) || (xCoordinate == 2 && yCoordinate == 1) || (xCoordinate == 1 && yCoordinate == 2) || (xCoordinate == 2 && yCoordinate == 2))
			{
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[start];
			}
			else if(xCoordinate == numTilesX -2 && yCoordinate == numTilesY -2)
			{
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[finish];
			}
			else if(xCoordinate == 0 || xCoordinate == (numTilesX - 1) || yCoordinate == 0 || yCoordinate == (numTilesY - 1) || randomPercentage <= 1) //if we are on the edge OR if the tile is randomly stone
			{
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];				
			}
			else
			{
				int randomTurretPlacementPercentage;
				int randomTankPlacementPercentage;
				if(m_numEnemyTurretSpawnOdds == 0)
				{
					randomTurretPlacementPercentage = 0;
				}
				else
				{
					randomTurretPlacementPercentage= GetRandomIntInRange(1, m_numEnemyTurretSpawnOdds); //TODO:Turrets have a 10% chance of spawning for now
				}

				if(m_numEnemyTankSpawnOdds == 0)
				{
					randomTankPlacementPercentage = 0;
				}
				else
				{
					randomTankPlacementPercentage = GetRandomIntInRange(1, m_numEnemyTankSpawnOdds);
				}				
				
				if(randomTurretPlacementPercentage == 1)
				{
					InitializeEnemyTurret(newTile.m_tileCoords);
				}
				else if(randomTankPlacementPercentage == 1)
				{
					InitializeEnemyTank(newTile.m_tileCoords);
				}
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_groundTileType];	
			}

			m_tiles.push_back(newTile);
		}		
	}
}

void Map::GenerateBossMap(int numTilesX, int numTilesY)
{
	for(int xCoordinate = 0; xCoordinate < numTilesX; xCoordinate++)
	{	
		for(int yCoordinate = 0; yCoordinate < numTilesY; yCoordinate++)
		{
			Tile newTile;

			newTile.m_tileCoords = IntVector2(xCoordinate, yCoordinate);
			newTile.m_tileBounds = AABB2(Vector2((float)xCoordinate, (float)yCoordinate),Vector2((float)xCoordinate + 1.f, (float)yCoordinate + 1.f)); //tiles are 1 across			

			if((xCoordinate == 1 && yCoordinate == 1) || (xCoordinate == 2 && yCoordinate == 1) || (xCoordinate == 1 && yCoordinate == 2) || (xCoordinate == 2 && yCoordinate == 2))
			{
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[start];
			}
			
			else if(xCoordinate == 0 || xCoordinate == (numTilesX - 1) || yCoordinate == 0 || yCoordinate == (numTilesY - 1)) //if we are on the edge OR if the tile is randomly stone
			{
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];				
			}
			else
			{
				newTile.m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_groundTileType];	
			}

			m_tiles.push_back(newTile);
		}		
	}

	m_tiles[104].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[105].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[124].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[125].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[264].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[265].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[284].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[285].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	
	m_tiles[315].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[316].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[335].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[336].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[116].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[115].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[96].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];
	m_tiles[95].m_tileDefinition = &TileDefinition::s_tileDefinitions[this->m_solidTileType];

	InitializeBossTank( IntVector2(10, 10));
}

void Map::InstantiatePlayerBullet()
{
	Bullet* bullet = new Bullet();
	bullet->m_isDead = false;
	bullet->m_Type = playerbullet;
	bullet->m_health = 3.f;
	
	bullet->m_radius = 1.f;
	bullet->m_cosmeticRadius = .05f;
	bullet->m_color = Rgba(0.f, 0.f, 1.f, 1.f);
	bullet->m_rotationDegrees = m_playerTankEntities[0]->m_turretRotationDegrees; //start bullet facing the direction of the turret
	bullet->m_rotationSpeed = 0.f;
	
	bullet->m_movementSpeed = 10.f;
	bullet->m_movementVector = Vector2(CosDegrees(m_playerTankEntities[0]->m_turretRotationDegrees), SinDegrees(m_playerTankEntities[0]->m_turretRotationDegrees)).GetNormalized();
	bullet->m_center = m_playerTankEntities[0]->m_center + (bullet->m_movementVector * .3f);

	bullet->m_textureBounds = AABB2(Vector2(0.f, 0.f), 1, 1);
	bullet->m_texture = "Data/Images/Bullet.png";
	g_theRenderer->CreateOrGetTexture(bullet->m_texture );

	bullet->m_age = 0.f;

	m_bulletEntities.push_back(bullet);
}

void Map::InstantiateEnemyBullet(float rotationDegrees, Vector2 centerOfFiringObject)
{
	Bullet* bullet = new Bullet();
	bullet->m_isDead = false;
	bullet->m_Type = enemybullet;
	bullet->m_health = 1.f;

	bullet->m_radius = 1.f;
	bullet->m_cosmeticRadius = .05f;
	bullet->m_color = Rgba(1.f, 0.f, 0.f, 1.f);
	bullet->m_rotationDegrees = rotationDegrees; //start bullet facing the direction of the turret
	bullet->m_rotationSpeed = 0.f;

	bullet->m_movementSpeed = 5.f;
	bullet->m_movementVector = Vector2(CosDegrees(rotationDegrees), SinDegrees(rotationDegrees)).GetNormalized();
	bullet->m_center = centerOfFiringObject + (bullet->m_movementVector * .3f);

	bullet->m_textureBounds = AABB2(Vector2(0.f, 0.f), 1, 1);
	bullet->m_texture = "Data/Images/Bullet.png";
	g_theRenderer->CreateOrGetTexture(bullet->m_texture );

	bullet->m_age = 0.f;

	m_bulletEntities.push_back(bullet);
}

void Map::InstantiateExplosion(Vector2 location, float radius, float duration)
{
	Explosion* explosion = new Explosion(location, radius, duration);

	m_explosions.push_back(explosion);
}

void Map::InstantiateAirStrike(Vector2 location, float radius, float duration)
{
	AirStrike* airStrike = new AirStrike(location, radius, duration);

	m_airstrikes.push_back(airStrike);
}

void Map::InitializeEnemyTurret(IntVector2 spawnTileCoordinates)
{		
	IntVector2 spawnTile = IntVector2(0, 0);
	spawnTile = spawnTileCoordinates;

	Turret* newTurret = new Turret();

	newTurret->m_Type = enemyturret;
	newTurret->m_isReadyToShoot = true;
	newTurret->m_doesHaveTargetLineOfSight = false;
	newTurret->m_health = 1.f;
	newTurret->m_isDead = false;
	newTurret->m_goalCooldown = 0.f;
	newTurret->m_shootCooldown = 0.f;
	newTurret->m_radius = 1.f;	
	newTurret->m_center = Vector2((float)spawnTile.x + .5f, (float)spawnTile.y + .5f);//move the tank to the center of the tile	
	newTurret->m_cosmeticRadius = .4f;
	newTurret->m_turretLaserDrawDistance = g_turretLaserDistance; 
		
	newTurret->m_turretRotationDegrees = GetRandomFloatInRange(0.f, 360.f);
	newTurret->m_age = 0.f;
	newTurret->m_color = Rgba(1.f, 1.f, 1.f, 1.f);
	newTurret->m_turretOrientationVector = Vector2(CosDegrees(newTurret->m_turretRotationDegrees), SinDegrees(newTurret->m_turretRotationDegrees)).GetNormalized();
	newTurret->m_textureBounds = AABB2(Vector2(0.f, 0.f), newTurret->m_radius, newTurret->m_radius);
	newTurret->m_baseTexture = "Data/Images/EnemyTurretBase.png";
	newTurret->m_turretTexture = "Data/Images/EnemyTurretTop.png";	

	g_theRenderer->CreateOrGetTexture(newTurret->m_baseTexture);
	g_theRenderer->CreateOrGetTexture(newTurret->m_turretTexture);

	newTurret->m_innerCollider.radius = 0.8f * newTurret->m_cosmeticRadius;
	newTurret->m_outerCollider.radius = 1.f * newTurret->m_cosmeticRadius;

	m_enemyTurretEntities.push_back(newTurret);	
}

void Map::InitializeEnemyTank(IntVector2 spawnTileCoordinates)
{		
	IntVector2 spawnTile = IntVector2(0, 0);
	spawnTile = spawnTileCoordinates;

	EnemyTank* newTank = new EnemyTank();

	newTank->m_Type = enemytank;
	newTank->m_isReadyToShoot = true;
	newTank->m_doesHaveTargetLineOfSight = false;
	newTank->m_health = 3.f;
	newTank->m_isDead = false;
	newTank->m_shootCooldown = 0.f;
	newTank->m_goalCooldown = 0.f;
	newTank->m_radius = 1.f;	
	newTank->m_center = Vector2((float)spawnTile.x + .5f, (float)spawnTile.y + .5f);//move the tank to the center of the tile	
	newTank->m_cosmeticRadius = .5f;
	newTank->m_rotationDegrees = 90.f; //start tank facing up	
	newTank->m_movementSpeed = .5f;
	newTank->m_movementVector = Vector2(0,0);
	newTank->m_orientationVector = Vector2(CosDegrees(newTank->m_rotationDegrees), SinDegrees(newTank->m_rotationDegrees)).GetNormalized();
	newTank->m_age = 0.f;
	newTank->m_targetPosition = Vector2(-1, -1);
	newTank->m_color = Rgba(1.f, 1.f, 1.f, 1.f);
	newTank->m_textureBounds = AABB2(Vector2(0.f, 0.f), newTank->m_radius, newTank->m_radius);
	newTank->m_texture = "Data/Images/EnemyTank4.png";

	g_theRenderer->CreateOrGetTexture(newTank->m_texture);

	newTank->m_innerCollider.radius = 0.8f * newTank->m_cosmeticRadius;
	newTank->m_outerCollider.radius = 1.f * newTank->m_cosmeticRadius;

	m_enemyTankEntities.push_back(newTank);	
}

void Map::InitializeBossTank(IntVector2 spawnTileCoordinates)
{		
	IntVector2 spawnTile = IntVector2(0, 0);
	spawnTile = spawnTileCoordinates;

	BossTank* newTank = new BossTank();

	newTank->m_Type = enemytank;
	newTank->m_isReadyToShoot = true;
	newTank->m_doesHaveTargetLineOfSight = false;
	newTank->m_health = 100.f;
	newTank->m_isDead = false;
	newTank->m_shootCooldown = 0.f;
	newTank->m_goalCooldown = 0.f;
	newTank->m_attackMode = normalFire;
	newTank->m_airStrikeModeTimer = 0.f;
	newTank->m_shootCount = 0.0;
	newTank->m_radius = 4.f;	
	newTank->m_center = Vector2((float)spawnTile.x + .5f, (float)spawnTile.y + .5f);//move the tank to the center of the tile	
	newTank->m_cosmeticRadius = .5f;
	newTank->m_rotationDegrees = 90.f; //start tank facing up	
	/*newTank->m_movementSpeed = .5f;
	newTank->m_movementVector = Vector2(0,0);*/
	newTank->m_orientationVector = Vector2(CosDegrees(newTank->m_rotationDegrees), SinDegrees(newTank->m_rotationDegrees)).GetNormalized();
	newTank->m_age = 0.f;
	newTank->m_targetPosition = Vector2(-1, -1);
	newTank->m_color = Rgba(1.f, 1.f, 1.f, 1.f);
	newTank->m_textureBounds = AABB2(Vector2(0.f, 0.f), newTank->m_radius, newTank->m_radius);
	newTank->m_texture = "Data/Images/EnemyTank3.png";

	g_theRenderer->CreateOrGetTexture(newTank->m_texture);

	newTank->m_innerCollider.radius = 0.8f * newTank->m_cosmeticRadius;
	newTank->m_outerCollider.radius = 1.f * newTank->m_cosmeticRadius;

	m_bossEntities.push_back(newTank);	
}

void Map::IntializePlayerTank(PlayerTank* newPlayer)
{
	IntVector2 spawnTile = IntVector2(0, 0);
	
	for(size_t i = 0; i < m_tiles.size(); i++) //find the first grass tile and spawn the tank there
	{
		if(m_tiles[i].m_tileDefinition->m_isSolid == false)
		{
			spawnTile = m_tiles[i].m_tileCoords;
			break;
		}
	}
	newPlayer->m_Type = playertank;
	newPlayer->m_isReadyToShoot = true;
	newPlayer->m_isAccelerating = false;
	newPlayer->m_health = 10.f;
	newPlayer->m_isDead = false;
	newPlayer->m_shootCooldown = 0.f;
	newPlayer->m_radius = 1.f;	
	newPlayer->m_center = Vector2((float)spawnTile.x + .5f, (float)spawnTile.y + .5f);//move the tank to the center of the tile	
	newPlayer->m_cosmeticRadius = .5f;
	newPlayer->m_rotationDegrees = 90.f; //start tank facing up
	newPlayer->m_turretRotationDegrees = 90.f;
	newPlayer->m_rotationSpeed = 0.0f; //set to 0. this will change when the player turns the ship
	newPlayer->m_movementSpeed = 1.f;
	newPlayer->m_age = 0.f;
	newPlayer->m_movementVector = Vector2(1,0);
	newPlayer->m_color = Rgba(1.f, 1.f, 1.f, 1.f);
	newPlayer->m_orientationVector = Vector2(CosDegrees(newPlayer->m_rotationDegrees), SinDegrees(newPlayer->m_rotationDegrees)).GetNormalized();
	newPlayer->m_turretOrientationVector = Vector2(CosDegrees(newPlayer->m_turretRotationDegrees), SinDegrees(newPlayer->m_turretRotationDegrees)).GetNormalized();

	newPlayer->m_tankBounds = AABB2(Vector2(0.f, 0.f), newPlayer->m_radius, newPlayer->m_radius);
	newPlayer->m_baseTexture = g_theRenderer->CreateOrGetTexture("Data/Images/PlayerTankBase.png");
	newPlayer->m_turretTexture = g_theRenderer->CreateOrGetTexture("Data/Images/PlayerTankTop.png");	

	newPlayer->m_innerCollider.radius = 0.8f * newPlayer->m_cosmeticRadius;
	newPlayer->m_outerCollider.radius = 1.f * newPlayer->m_cosmeticRadius;

	m_playerTankEntities.push_back(newPlayer);
}

void Map::BulletToTileCollision(Bullet* bullet, float timeDelta)
{
	IntVector2 currentPositionTile = ConvertVector2ToIntVector2(bullet->m_center);

	Vector2 nextPosition = bullet->m_center + (bullet->m_movementVector * timeDelta * bullet->m_movementSpeed);
	IntVector2 nextPositionTile = ConvertVector2ToIntVector2(nextPosition);
	int nextTileIndex =  (m_numTilesX * nextPositionTile.x) + nextPositionTile.y;

	if(m_tiles[nextTileIndex].m_tileDefinition->m_isSolid)
	{
		if(currentPositionTile.x != nextPositionTile.x)
		{
			bullet->m_movementVector.x *= -1.f;			
		}

		if(currentPositionTile.y != nextPositionTile.y)
		{
			bullet->m_movementVector.y *= -1.f;
		}

		bullet->m_rotationDegrees = bullet->m_movementVector.GetOrientationDegrees();
		bullet->m_health--;
		if(bullet->m_health == 0)
		{
			InstantiateExplosion(bullet->m_center, .5f, .5f);
		}
	}
	else
	{
		bullet->Update(timeDelta);
	}
}

void Map::PlayerTankToTileCollision()
{
	for(size_t i = 0; i < m_playerTankEntities.size(); i++)
	{
		PlayerTank*& tank = m_playerTankEntities[i];

		IntVector2 tileOfTank = ConvertVector2ToIntVector2(tank->m_center);

		IntVector2 eastTile = IntVector2(tileOfTank.x + 1, tileOfTank.y);
		IntVector2 westTile = IntVector2(tileOfTank.x - 1 , tileOfTank.y);
		IntVector2 northTile = IntVector2(tileOfTank.x, tileOfTank.y + 1);
		IntVector2 southTile = IntVector2(tileOfTank.x, tileOfTank.y - 1);
		IntVector2 northEastTile = IntVector2(tileOfTank.x + 1, tileOfTank.y + 1);
		IntVector2 northWestTile = IntVector2(tileOfTank.x - 1, tileOfTank.y + 1);
		IntVector2 southEastTile = IntVector2(tileOfTank.x + 1, tileOfTank.y - 1);
		IntVector2 southWestTile = IntVector2(tileOfTank.x - 1, tileOfTank.y - 1);

		DetectPlayerToTileCollision(tank->m_center, northEastTile, northeast); 
		DetectPlayerToTileCollision(tank->m_center, northWestTile, northwest); 
		DetectPlayerToTileCollision(tank->m_center, southEastTile, southeast); 
		DetectPlayerToTileCollision(tank->m_center, southWestTile, southwest); 
		DetectPlayerToTileCollision(tank->m_center, eastTile, east); 
		DetectPlayerToTileCollision(tank->m_center, westTile, west); 
		DetectPlayerToTileCollision(tank->m_center, northTile, north); 
		DetectPlayerToTileCollision(tank->m_center, southTile, south);		
	}
}

void Map::EnemyTankToTileCollision()
{
	for(size_t i = 0; i < m_enemyTankEntities.size(); i++)
	{
		if(m_enemyTankEntities[i] != nullptr)
		{
			if(!m_enemyTankEntities[i]->m_isDead)
			{
				EnemyTank*& tank = m_enemyTankEntities[i];

				IntVector2 tileOfTank = ConvertVector2ToIntVector2(tank->m_center);

				IntVector2 eastTile = IntVector2(tileOfTank.x + 1, tileOfTank.y);
				IntVector2 westTile = IntVector2(tileOfTank.x - 1 , tileOfTank.y);
				IntVector2 northTile = IntVector2(tileOfTank.x, tileOfTank.y + 1);
				IntVector2 southTile = IntVector2(tileOfTank.x, tileOfTank.y - 1);
				IntVector2 northEastTile = IntVector2(tileOfTank.x + 1, tileOfTank.y + 1);
				IntVector2 northWestTile = IntVector2(tileOfTank.x - 1, tileOfTank.y + 1);
				IntVector2 southEastTile = IntVector2(tileOfTank.x + 1, tileOfTank.y - 1);
				IntVector2 southWestTile = IntVector2(tileOfTank.x - 1, tileOfTank.y - 1);

				DetectEntityToTileCollision(i, tank->m_center, northEastTile, northeast); 
				DetectEntityToTileCollision(i, tank->m_center, northWestTile, northwest); 
				DetectEntityToTileCollision(i, tank->m_center, southEastTile, southeast); 
				DetectEntityToTileCollision(i, tank->m_center, southWestTile, southwest); 
				DetectEntityToTileCollision(i, tank->m_center, eastTile, east); 
				DetectEntityToTileCollision(i, tank->m_center, westTile, west); 
				DetectEntityToTileCollision(i, tank->m_center, northTile, north); 
				DetectEntityToTileCollision(i, tank->m_center, southTile, south);	
			}			
		}			
	}
}

void Map::BulletToEntityCollision()
{
	for(size_t bulletIndex = 0; bulletIndex < m_bulletEntities.size(); bulletIndex++)
	{
		if(m_bulletEntities[bulletIndex] != nullptr)
		{
			if(!m_bulletEntities[bulletIndex]->m_isDead)
			{
				if(m_bulletEntities[bulletIndex]->m_Type == playerbullet)
				{
					for(size_t enemyTurretIndex = 0; enemyTurretIndex < m_enemyTurretEntities.size(); enemyTurretIndex++)
					{
						if(m_enemyTurretEntities[enemyTurretIndex] != nullptr)
						{
							if(!m_enemyTurretEntities[enemyTurretIndex]->m_isDead)
							{
								bool hasCollided = m_enemyTurretEntities[enemyTurretIndex]->m_innerCollider.IsPointInside(m_bulletEntities[bulletIndex]->m_center); 
								if(hasCollided)
								{
									m_bulletEntities[bulletIndex]->m_isDead = true;
									SoundID soundHit = g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
									g_theAudio->PlaySound( soundHit );
									InstantiateExplosion(m_bulletEntities[bulletIndex]->m_center, .5f, .5);
									m_enemyTurretEntities[enemyTurretIndex]->m_health--;
									if(m_enemyTurretEntities[enemyTurretIndex]->m_health == 0)
									{
										InstantiateExplosion(m_enemyTurretEntities[enemyTurretIndex]->m_center, 1.f, .5);
										SoundID deadSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDied.wav");
										g_theAudio->PlaySound( deadSound );
									}
								}
							}
						}	
					}
					if(!m_bulletEntities[bulletIndex]->m_isDead) //need to check again to make sure it's not dead from hitting a turret
					{
						for(size_t enemyTankIndex = 0; enemyTankIndex < m_enemyTankEntities.size(); enemyTankIndex++)
						{
							if(m_enemyTankEntities[enemyTankIndex] != nullptr)
							{
								if(!m_enemyTankEntities[enemyTankIndex]->m_isDead)
								{
									bool hasCollided = m_enemyTankEntities[enemyTankIndex]->m_innerCollider.IsPointInside(m_bulletEntities[bulletIndex]->m_center); 
									if(hasCollided)
									{
										m_bulletEntities[bulletIndex]->m_isDead = true;
										SoundID hitSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
										g_theAudio->PlaySound( hitSound );
										InstantiateExplosion(m_bulletEntities[bulletIndex]->m_center, .5f, .5);
										m_enemyTankEntities[enemyTankIndex]->m_health--;
										if(m_enemyTankEntities[enemyTankIndex]->m_health == 0)
										{
											InstantiateExplosion(m_enemyTankEntities[enemyTankIndex]->m_center, 1.f, .5);
											SoundID deadSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDied.wav");
											g_theAudio->PlaySound( deadSound );
										}
									}
								}
							}	
						}
					}

					if(!m_bulletEntities[bulletIndex]->m_isDead) //need to check again to make sure it's not dead from hitting a turret
					{
						for(size_t enemyBossIndex = 0; enemyBossIndex < m_bossEntities.size(); enemyBossIndex++)
						{
							if(m_bossEntities[enemyBossIndex] != nullptr)
							{
								if(!m_bossEntities[enemyBossIndex]->m_isDead)
								{
									bool hasCollided = m_bossEntities[enemyBossIndex]->m_innerCollider.IsPointInside(m_bulletEntities[bulletIndex]->m_center); 
									if(hasCollided)
									{
										m_bulletEntities[bulletIndex]->m_isDead = true;
										SoundID hitSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
										g_theAudio->PlaySound( hitSound );
										InstantiateExplosion(m_bulletEntities[bulletIndex]->m_center, .5f, .5);
										m_bossEntities[enemyBossIndex]->m_health--;
										if(m_bossEntities[enemyBossIndex]->m_health == 0)
										{
											SoundID deadSound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerDied.wav");
											g_theAudio->PlaySound( deadSound );
											InstantiateExplosion(m_bossEntities[enemyBossIndex]->m_center, 4.5f, 1.f);
											InstantiateExplosion(m_bossEntities[enemyBossIndex]->m_center, 2.f, .5f);
											InstantiateExplosion(m_bossEntities[enemyBossIndex]->m_center, 3.f, 1.f);
											InstantiateExplosion(m_bossEntities[enemyBossIndex]->m_center, 2.5f, 2.f);
											InstantiateExplosion(m_bossEntities[enemyBossIndex]->m_center, 4.5f, 1.f);
										}
									}
								}
							}	
						}
					}

				}	
				else if(m_bulletEntities[bulletIndex]->m_Type == enemybullet)
				{					
					if(m_playerTankEntities[0] != nullptr)
					{
						if(!m_playerTankEntities[0]->m_isDead)
						{
							bool hasCollided = m_playerTankEntities[0]->m_innerCollider.IsPointInside(m_bulletEntities[bulletIndex]->m_center); 
							if(hasCollided)
							{
								SoundID hitSound = g_theAudio->CreateOrGetSound("Data/Audio/Playerhit.wav");
								g_theAudio->PlaySound( hitSound );
								InstantiateExplosion(m_bulletEntities[bulletIndex]->m_center, .5f, .5);
								m_bulletEntities[bulletIndex]->m_isDead = true;
								m_playerTankEntities[0]->m_health--;
								if(m_playerTankEntities[0]->m_health == 0)
								{
									SoundID deadSound = g_theAudio->CreateOrGetSound("Data/Audio/GameOver.mp3");
									g_theAudio->PlaySound( deadSound );
									InstantiateExplosion(m_playerTankEntities[0]->m_center, 1.f, .5);
								}
							}
						}
					}							
				}
			}
		}			
	}
}

void Map::EntityToEntityCollision()
{
	//first check player to turret (CURRENTLY ONLY RUNS ONCE BECAUSE ONLY ONE PLAYER) - TODO
	for(size_t playerTankIndex = 0; playerTankIndex < m_playerTankEntities.size(); playerTankIndex++)
	{
		if(m_playerTankEntities[playerTankIndex] != nullptr)
		{
			if(!m_playerTankEntities[playerTankIndex]->m_isDead)
			{
				for(size_t enemyTurretIndex = 0; enemyTurretIndex < m_enemyTurretEntities.size(); enemyTurretIndex++)
				{
					if(m_enemyTurretEntities[enemyTurretIndex] != nullptr)
					{
						if(!m_enemyTurretEntities[enemyTurretIndex]->m_isDead)
						{
							bool hasCollided = DoDiscsOverlap(m_enemyTurretEntities[enemyTurretIndex]->m_innerCollider, m_playerTankEntities[0]->m_innerCollider); 
							if(hasCollided)
							{
								float displacement = GetDistance(m_playerTankEntities[0]->m_center, m_enemyTurretEntities[enemyTurretIndex]->m_center);
								float amountToPush = displacement - (m_playerTankEntities[0]->m_innerCollider.radius + m_enemyTurretEntities[enemyTurretIndex]->m_innerCollider.radius);

								amountToPush = fabsf(amountToPush);

								Vector2 pushOutDirection = (m_playerTankEntities[0]->m_center - m_enemyTurretEntities[enemyTurretIndex]->m_center).GetNormalized();

								m_playerTankEntities[0]->m_center += pushOutDirection * (amountToPush);
							}
						}
					}				
				}	

				for(size_t enemyTankIndex = 0; enemyTankIndex < m_enemyTankEntities.size(); enemyTankIndex++)
				{
					if(m_enemyTankEntities[enemyTankIndex] != nullptr)
					{
						if(!m_enemyTankEntities[enemyTankIndex]->m_isDead)
						{
							bool hasCollided = DoDiscsOverlap(m_enemyTankEntities[enemyTankIndex]->m_innerCollider, m_playerTankEntities[0]->m_innerCollider); 
							if(hasCollided)
							{
								float displacement = GetDistance(m_playerTankEntities[0]->m_center, m_enemyTankEntities[enemyTankIndex]->m_center);
								float amountToPush = displacement - (m_playerTankEntities[0]->m_innerCollider.radius + m_enemyTankEntities[enemyTankIndex]->m_innerCollider.radius);

								amountToPush = fabsf(amountToPush);

								Vector2 pushOutDirection = (m_playerTankEntities[0]->m_center - m_enemyTankEntities[enemyTankIndex]->m_center).GetNormalized();

								m_playerTankEntities[0]->m_center += pushOutDirection * (amountToPush * .5f);
								m_enemyTankEntities[enemyTankIndex]->m_center += (-1 * pushOutDirection) * (amountToPush * .5f);
							}
						}
					}				
				}	

				for(size_t bossTankIndex = 0; bossTankIndex < m_bossEntities.size(); bossTankIndex++)
				{
					if(m_bossEntities[bossTankIndex] != nullptr)
					{
						if(!m_bossEntities[bossTankIndex]->m_isDead)
						{
							bool hasCollided = DoDiscsOverlap(m_bossEntities[bossTankIndex]->m_innerCollider, m_playerTankEntities[0]->m_innerCollider); 
							if(hasCollided)
							{
								float displacement = GetDistance(m_playerTankEntities[0]->m_center, m_bossEntities[bossTankIndex]->m_center);
								float amountToPush = displacement - (m_playerTankEntities[0]->m_innerCollider.radius + m_bossEntities[bossTankIndex]->m_innerCollider.radius);

								amountToPush = fabsf(amountToPush);

								Vector2 pushOutDirection = (m_playerTankEntities[0]->m_center - m_bossEntities[bossTankIndex]->m_center).GetNormalized();

								m_playerTankEntities[0]->m_center += pushOutDirection * (amountToPush * .5f);
								m_bossEntities[bossTankIndex]->m_center += (-1 * pushOutDirection) * (amountToPush * .5f);
							}
						}
					}				
				}
			}
		}	
	}

	for(size_t enemyTankIndex = 0; enemyTankIndex < m_enemyTankEntities.size(); enemyTankIndex++)
	{
		if(m_enemyTankEntities[enemyTankIndex] != nullptr)
		{
			if(!m_enemyTankEntities[enemyTankIndex]->m_isDead)
			{
				for(size_t nextTankIndex = 0; nextTankIndex < m_enemyTankEntities.size(); nextTankIndex++)
				{
					if(enemyTankIndex != nextTankIndex) //no need to check the same tank
					{
						if(m_enemyTankEntities[nextTankIndex] != nullptr)
						{
							if(!m_enemyTankEntities[nextTankIndex]->m_isDead)
							{
								bool hasCollided = DoDiscsOverlap(m_enemyTankEntities[nextTankIndex]->m_innerCollider, m_enemyTankEntities[enemyTankIndex]->m_innerCollider); 
								if(hasCollided)
								{
									float displacement = GetDistance(m_enemyTankEntities[enemyTankIndex]->m_center, m_enemyTankEntities[nextTankIndex]->m_center);
									float amountToPush = displacement - (m_enemyTankEntities[enemyTankIndex]->m_innerCollider.radius + m_enemyTankEntities[nextTankIndex]->m_innerCollider.radius);

									amountToPush = fabsf(amountToPush);

									Vector2 pushOutDirection = (m_enemyTankEntities[enemyTankIndex]->m_center - m_enemyTankEntities[nextTankIndex]->m_center).GetNormalized();

									m_enemyTankEntities[enemyTankIndex]->m_center += pushOutDirection * (amountToPush * .5f);
									m_enemyTankEntities[nextTankIndex]->m_center += (-1 * pushOutDirection) * (amountToPush * .5f);
								}
							}
						}
					}									
				}

				for(size_t enemyTurretIndex = 0; enemyTurretIndex < m_enemyTurretEntities.size(); enemyTurretIndex++)
				{
					if(m_enemyTurretEntities[enemyTurretIndex] != nullptr)
					{
						if(!m_enemyTurretEntities[enemyTurretIndex]->m_isDead)
						{
							bool hasCollided = DoDiscsOverlap(m_enemyTurretEntities[enemyTurretIndex]->m_innerCollider, m_enemyTankEntities[enemyTankIndex]->m_innerCollider); 
							if(hasCollided)
							{
								float displacement = GetDistance(m_enemyTankEntities[enemyTankIndex]->m_center, m_enemyTurretEntities[enemyTurretIndex]->m_center);
								float amountToPush = displacement - (m_enemyTankEntities[enemyTankIndex]->m_innerCollider.radius + m_enemyTurretEntities[enemyTurretIndex]->m_innerCollider.radius);

								amountToPush = fabsf(amountToPush);

								Vector2 pushOutDirection = (m_enemyTankEntities[enemyTankIndex]->m_center - m_enemyTurretEntities[enemyTurretIndex]->m_center).GetNormalized();

								m_enemyTankEntities[enemyTankIndex]->m_center += pushOutDirection * (amountToPush);
							}
						}
					}				
				}	
			}
		}
	}

	//checks enemy tanks to enemy turrets
	//checks enemy tanks to players
}

void Map::DetectPlayerToTileCollision(Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection)
{
	if(tileCoordinate.x >= 0 && tileCoordinate.y >= 0)
	{
		int tileIndex =  (m_numTilesX * tileCoordinate.x) + tileCoordinate.y;

		if(m_tiles[tileIndex].m_tileDefinition->m_isSolid)
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

			bool isColliding =  m_playerTankEntities[0]->m_innerCollider.IsPointInside(collisionPoint);
			if(isColliding)
			{
				float displacement = GetDistance(m_playerTankEntities[0]->m_center, collisionPoint);

				float amountToPush = m_playerTankEntities[0]->m_innerCollider.radius - displacement;
				Vector2 pushOutDirection = (m_playerTankEntities[0]->m_center - collisionPoint).GetNormalized();

				m_playerTankEntities[0]->m_center += pushOutDirection * (amountToPush);
			}
		}
		if(m_tiles[tileIndex].m_tileDefinition->m_tileType == finish)
		{
			g_didCompleteMap = true;
		}
	}	
}

void Map::DetectEntityToTileCollision(int tankIndex, Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection)
{
	if(tileCoordinate.x >= 0 && tileCoordinate.y >= 0)
	{
		int tileIndex =  (m_numTilesX * tileCoordinate.x) + tileCoordinate.y;

		if(m_tiles[tileIndex].m_tileDefinition->m_isSolid)
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

			bool isColliding =  m_enemyTankEntities[tankIndex]->m_innerCollider.IsPointInside(collisionPoint);
			if(isColliding)
			{
				float displacement = GetDistance(m_enemyTankEntities[tankIndex]->m_center, collisionPoint);

				float amountToPush = m_enemyTankEntities[tankIndex]->m_innerCollider.radius - displacement;
				Vector2 pushOutDirection = (m_enemyTankEntities[tankIndex]->m_center - collisionPoint).GetNormalized();

				m_enemyTankEntities[tankIndex]->m_center += pushOutDirection * (amountToPush);
			}
		}	
	}	
}

void Map::PlayerTankToAirStrikeCollisions()
{
	for(size_t playerTankIndex = 0; playerTankIndex < m_playerTankEntities.size(); playerTankIndex++)
	{
		if(m_playerTankEntities[playerTankIndex] != nullptr)
		{
			if(!m_playerTankEntities[playerTankIndex]->m_isDead)
			{			
				for(size_t airStrikeIndex = 0; airStrikeIndex < m_airstrikes.size(); airStrikeIndex++)
				{
					if(m_airstrikes[airStrikeIndex] != nullptr)
					{
						if(m_airstrikes[airStrikeIndex]->m_isExploding && m_airstrikes[airStrikeIndex]->m_readyForDelete == false)
						{
							float displacement = GetDistance(m_playerTankEntities[playerTankIndex]->m_center, m_airstrikes[airStrikeIndex]->m_position);
							if(displacement < m_playerTankEntities[playerTankIndex]->m_outerCollider.radius + (m_airstrikes[airStrikeIndex]->m_currentRadius/2.f))
							{
								InstantiateExplosion(m_playerTankEntities[playerTankIndex]->m_center, .5, 1.f);
								m_playerTankEntities[playerTankIndex]->m_health--;
								m_airstrikes[airStrikeIndex]->m_readyForDelete = true;;
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
	for(size_t i = 0; i < m_bulletEntities.size(); i++)
	{		
		if(m_bulletEntities[i] != nullptr)
		{
			if(m_bulletEntities[i]->m_isDead)
			{
				delete m_bulletEntities[i];
				m_bulletEntities[i] = nullptr;
			}
		}
	}

	for(size_t i = 0; i < m_enemyTurretEntities.size(); i++)
	{		
		if(m_enemyTurretEntities[i] != nullptr)
		{
			if(m_enemyTurretEntities[i]->m_isDead)
			{
				delete m_enemyTurretEntities[i];
				m_enemyTurretEntities[i] = nullptr;
			}
		}
	}

	for(size_t i = 0; i < m_enemyTankEntities.size(); i++)
	{		
		if(m_enemyTankEntities[i] != nullptr)
		{
			if(m_enemyTankEntities[i]->m_isDead)
			{
				delete m_enemyTankEntities[i];
				m_enemyTankEntities[i] = nullptr;
			}
		}
	}

	for(size_t i = 0; i < m_bossEntities.size(); i++)
	{		
		if(m_bossEntities[i] != nullptr)
		{
			if(m_bossEntities[i]->m_isDead)
			{
				delete m_bossEntities[i];
				m_bossEntities[i] = nullptr;
			}
		}
	}

	for(size_t i = 0; i < m_airstrikes.size(); i++)
	{		
		if(m_airstrikes[i] != nullptr)
		{
			if(m_airstrikes[i]->m_readyForDelete)
			{
				delete m_airstrikes[i];
				m_airstrikes[i] = nullptr;
			}
		}
	}

	for(size_t i = 0; i < m_explosions.size(); i++)
	{		
		if(m_explosions[i] != nullptr)
		{
			if(m_explosions[i]->m_animation->IsFinished())
			{
				delete m_explosions[i];
				m_explosions[i] = nullptr;
			}
		}
	}
		
}

bool Map::HasLineOfSight(const Vector2& startPos, const Vector2& endPos) const
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

RaycastResult Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance ) const
{
	RaycastResult result;
	float numSteps = maxDistance * g_raycastStepsPerTile;	
	float singleStep = 1/g_raycastStepsPerTile;

	IntVector2 previousTileCoords = ConvertVector2ToIntVector2(startPos);

	for(float stepIndex = 0; stepIndex <= numSteps; stepIndex++)
	{
		Vector2 currentPosition = startPos + (direction * (stepIndex * singleStep));
		IntVector2 currentTileCoordinates = ConvertVector2ToIntVector2(currentPosition);

		int currentTileIndex = (m_numTilesX * currentTileCoordinates.x) + currentTileCoordinates.y;
		if(m_tiles[currentTileIndex].m_tileDefinition->m_isSolid)
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
