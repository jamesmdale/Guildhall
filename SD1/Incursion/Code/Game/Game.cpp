#include "Game.hpp"
#include <stdlib.h>
#include "Engine\Math\MathUtils.hpp"
#include "engine\renderer\renderer.hpp"
#include "game\GameCommon.hpp"
#include "Map.hpp"
#include "PlayerTank.hpp"
#include "Camera.hpp"
#include "TileDefinition.hpp"


Game* g_Game = nullptr;
PlayerTank* playerTank = nullptr;
bool areMapsInitialized = false;
std::vector<Map*> m_maps;
int currentLevelIndex = 0;


Game::Game()
{
	SoundID sound = g_theAudio->CreateOrGetSound("Data/Audio/Anticipation.mp3");
	g_theAudio->PlaySound( sound );
}

Game::~Game()
{

}

void Game::Initialize()
{	
	
}

void Game::Update(float timeDelta)
{	

	if(g_isGodMode && playerTank != nullptr)
	{
		playerTank->m_health = 999999.f;
		g_isGodMode = false;
	}

	float modifiedtimeDelta = timeDelta;
	
	if(g_wasPlayStateJustSet)
	{		
		g_wasPlayStateJustSet = false;
		g_gameState = play;

		if(areMapsInitialized)
		{
			for(int i = 0; i < g_mapCount; i++)
			{				
				m_maps[i]->m_enemyTurretEntities.clear();
				m_maps[i]->m_tiles.clear();
				m_maps[i]->m_enemyTankEntities.clear();
				m_maps[i]->m_playerTankEntities.clear();
				m_maps[i]->m_bulletEntities.clear();
				m_maps[i]->m_explosions.clear();
				if(m_maps[i]->m_numBosses > 0)
				{
					m_maps[i]->GenerateBossMap(m_maps[i]->m_numTilesX, m_maps[i]->m_numTilesY);
				}
				else
				{
					m_maps[i]->GenerateMap(m_maps[i]->m_numTilesX, m_maps[i]->m_numTilesY);
				}
				
			}
			currentLevelIndex = 0;
			m_maps[currentLevelIndex]->IntializePlayerTank(playerTank);	
		}			
	}
	
	if(g_gameState == attract)
	{	
		modifiedtimeDelta = UpdateFlagsFromInput(timeDelta); //edit ship values and flip flags based on inputs. Update time delta
	}
	else
	{
		if(!areMapsInitialized)
		{	

			playerTank = new PlayerTank();
			Map* mapOne = new Map();
			mapOne->m_groundTileType = grass;
			mapOne->m_solidTileType = stone;
			mapOne->m_numTilesX = 20;
			mapOne->m_numTilesY = 20;
			mapOne->m_numEnemyTankSpawnOdds = 60;//1/60
			mapOne->m_numEnemyTurretSpawnOdds = 60; //1/60			
			mapOne->GenerateMap(mapOne->m_numTilesX, mapOne->m_numTilesY);			
			mapOne->IntializePlayerTank(playerTank);
			m_maps.push_back((mapOne));	

			Map* mapTwo  = new Map();			
			//mapTwo->IntializePlayerTank(tank);
			mapTwo->m_groundTileType = water;
			mapTwo->m_solidTileType = barricade;
			mapTwo->m_numTilesX = 30;
			mapTwo->m_numTilesY = 30;
			mapTwo->m_numEnemyTankSpawnOdds = 40;  //1/40
			mapTwo->m_numEnemyTurretSpawnOdds = 40; //1/40
			mapTwo->GenerateMap(mapTwo->m_numTilesX, mapTwo->m_numTilesY);	
			m_maps.push_back((mapTwo));	

			Map* mapThree = new Map();						
			//mapThree->IntializePlayerTank(tank);
			mapThree->m_groundTileType = sand;
			mapThree->m_solidTileType = sandstone;
			mapThree->m_numTilesX = 20;
			mapThree->m_numTilesY = 20;
			mapThree->m_numEnemyTankSpawnOdds = 0; //0%
			mapThree->m_numEnemyTurretSpawnOdds = 0; //0%
			mapThree->m_numBosses = 1;
			mapThree->GenerateBossMap(mapThree->m_numTilesX, mapThree->m_numTilesY);
			m_maps.push_back((mapThree));	

			
			areMapsInitialized = true;
		}

		if(g_didCompleteMap == true)
		{
			g_didCompleteMap = false;
			currentLevelIndex++;

			float currentHealth = playerTank->m_health;

			m_maps[currentLevelIndex]->IntializePlayerTank(playerTank);
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_health = currentHealth;
		}

		modifiedtimeDelta = UpdateFlagsFromInput(timeDelta); //edit ship values and flip flags based on inputs. Update time delta
		
		m_maps[currentLevelIndex]->Update(modifiedtimeDelta);
	}	
	
}

void Game::Render()
{	
	if(g_gameState == attract)
	{
		Vector2 bottomLeftOrtho = Vector2(0.f, 0.f);
		Vector2 topRightOrtho = Vector2(50.f, 50.f);	

		g_theRenderer->SetOrtho(bottomLeftOrtho, topRightOrtho);
		g_theRenderer->ClearColor(g_backgroundColor);
		g_theRenderer->Clear();

		Texture* attractScreen = g_theRenderer->CreateOrGetTexture("Data/Images/IncursionScreenBlack.png");
		g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *attractScreen, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
	}	
	else if (!g_wasPlayStateJustSet)
	{
		m_maps[currentLevelIndex]->Render();	
	}
}

float Game::UpdateFlagsFromInput(float timeDelta)
{	
	
	XboxController inputController = g_theInput->GetController(0);
	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_T))
	{
		g_isSlowed = true;

		if(g_isSlowed)
		{
			timeDelta *= 0.1f;		
		}
	}

	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ESCAPE)))
	{
		if(g_gameState == attract)
		{
			g_isQuitting = true;
		}
		if(g_gameState == paused || g_gameState == respawnpaused)
		{
			g_gameState = attract;
		}
	}

	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_P) || inputController.WasKeyJustPressed(XBOX_START))
	{			
		if(g_gameState == attract)
		{
			g_wasPlayStateJustSet = true;			
		}
		else if(g_gameState == respawnpaused)
		{
			g_gameState = play;
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_isDead = false;
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_health = 10.f;
		}
		else if(g_isPausedDisabled == false)
		{
			if(g_gameState == paused)
			{
				g_wasPlayStateJustSet = true;
			}
			else
			{
				g_gameState = paused;	
			}	
		}		
	}

	if(g_gameState == paused || g_gameState == respawnpaused)
	{
		timeDelta = 0.0f;
	}
	
	//just getting the first inputController for now. Will extend for multiple tanks if necessary.
	
	if(g_gameState == play)
	{
		if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_F1))
		{
			if(g_isDebug == true)
			{
				g_isDebug = false;	
			}
			else
			{
				g_isDebug = true;	
			}
		}
		if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_F2))
		{
			g_isGodMode = true;
		}
		if(inputController.m_joyStickLeft.m_correctedPositionX  != 0 && inputController.m_joyStickLeft.m_correctedPositionY != 0)
		{
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_rotationDegrees = TurnToward(m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_rotationDegrees, inputController.m_joyStickLeft.m_angleDegrees, 180.f * timeDelta);
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_orientationVector = Vector2::MakeDirectionAtDegrees(m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_rotationDegrees);
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_movementVector = Vector2(inputController.m_joyStickLeft.m_correctedPositionX, inputController.m_joyStickLeft.m_correctedPositionY).GetNormalized() * inputController.m_joyStickLeft.m_magnitude;
		}
		else
		{
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_movementVector = Vector2(0,0);
		}

		if(inputController.m_joyStickRight.m_correctedPositionX  != 0 && inputController.m_joyStickRight.m_correctedPositionY != 0)
		{
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_turretRotationDegrees = TurnToward(m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_turretRotationDegrees, inputController.m_joyStickRight.m_angleDegrees, 360.f * timeDelta);
			m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_turretOrientationVector= Vector2::MakeDirectionAtDegrees(m_maps[currentLevelIndex]->m_playerTankEntities[0]->m_turretRotationDegrees);
		}

		if(inputController.m_rightTriggerValue > 0.5f)
		{
			g_isPlayerShooting = true;
		}
		else
		{
			g_isPlayerShooting = false;
		}
	}	

	if(g_wasPlayStateJustSet)
	{
		g_gameState = play;
	}
	return timeDelta;	
}

