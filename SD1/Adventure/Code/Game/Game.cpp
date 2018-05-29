#include "Game.hpp"
#include <stdlib.h>
#include "Engine/Math/MathUtils.hpp"
#include "engine/renderer/renderer.hpp"
#include "game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <vector>
#include "Game/Map.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/AdventureMapGenerationStep.hpp"
#include "Game/AdventureMapPortalGenerationStep.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Camera.hpp"

Game* g_Game = nullptr;
GameState g_currentState;
GameState g_transitionState;
float g_secondsInState;
float g_secondsTransistioning;
bool g_isFinishedTransistioning;

//TODO: Add input keys for viewing the whole map and etc.

Game::Game()
{
	InitializeGameConfig();
	InitializeFonts();
	InitializeTileDefinitions();
	InitializeItemDefinitions();
	InitializeActorDefinitions();
	InitializeProjectileDefinitions();
	InitializePortalDefinitions();	
	InitializeMapDefinitions();		
	InitializeAdventureDefinitions();


	g_currentState = ATTRACT;
	g_transitionState = NONE;
}

Game::~Game()
{

}

void Game::Initialize()
{
		
}

void Game::Update(float timeDelta)
{
	if(g_transitionState != NONE)
	{
		g_currentState = g_transitionState;
		g_transitionState = NONE;
		g_secondsInState = 0.f;
	}

	switch(g_currentState)
	{
	case ATTRACT:
		Update_Attract();
		break;
	case PLAYING:
		Update_Playing(timeDelta);
		break;
	case PAUSED:
		Update_Paused(timeDelta);
		break;
	case DEFEAT:
		Update_Defeat(timeDelta);
		break;
	case VICTORY:
		Update_Victory(timeDelta);
		break;	
	}
	g_secondsInState += timeDelta;

}

void Game::Render()
{
	switch(g_currentState)
	{
	case ATTRACT:
		Render_Attract();
		break;
	case PLAYING:
		Render_Playing();
		break;
	case PAUSED:
		Render_Paused();
		break;
	case DEFEAT:
		Render_Defeat();
		break;
	case VICTORY:
		Render_Victory();
		break;	
	}
}

void Game::BeginFrame()
{

}

void Game::EndFrame()
{
	//TODO: might put some transition logic in here for map change.
	ChangeMap();	
}


void Game::Update_Attract()
{
	m_maps.erase( m_maps.begin(), m_maps.end() );
	XboxController inputController = g_theInput->GetController(0);
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ESCAPE)))
	{
		g_isQuitting = true;
	}
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_RETURN)) ||  inputController.WasKeyJustPressed(XBOX_START))
	{
		GenerateAdventure("Balrog");
		g_transitionState = PLAYING;
	}
}
void Game::Update_Playing(float timeDelta)
{
	XboxController inputController = g_theInput->GetController(0);
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ESCAPE)) ||  inputController.WasKeyJustPressed(XBOX_START))
	{
		g_transitionState = PAUSED;
	}
	
	if(g_isPlayerAlive == false)
	{
		g_transitionState = DEFEAT;
	}

	m_currentMap->Update(timeDelta);
}
void  Game::Update_Paused(float timeDelta)
{
	XboxController inputController = g_theInput->GetController(0);
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ESCAPE)))
	{
		g_transitionState = ATTRACT;
	}
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_RETURN)) ||  inputController.WasKeyJustPressed(XBOX_START))
	{
		g_transitionState = PLAYING;
	}

	timeDelta = 0.0f;
	m_currentMap->Update(timeDelta);
}
void  Game::Update_Defeat(float timeDelta)
{
	XboxController inputController = g_theInput->GetController(0);
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ESCAPE)) || g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_RETURN)) ||  inputController.WasKeyJustPressed(XBOX_START))
	{
		g_transitionState = ATTRACT;
	}

	timeDelta = 0.0f;
	m_currentMap->Update(timeDelta);
}
void  Game::Update_Victory(float timeDelta)
{
	XboxController inputController = g_theInput->GetController(0);
	if(g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_ESCAPE)) || g_theInput->WasKeyJustPressed((g_theInput->KEYBOARD_RETURN)) ||  inputController.WasKeyJustPressed(XBOX_START))
	{
		g_transitionState = ATTRACT;
	}
	timeDelta = 0.0f;
	m_currentMap->Update(timeDelta);
}

void Game::Render_Attract()
{
	Vector2 bottomLeftOrtho = Vector2(0.f, 0.f);
	Vector2 topRightOrtho = Vector2(50.f, 50.f);	

	g_theRenderer->SetOrtho(bottomLeftOrtho, topRightOrtho);
	g_theRenderer->ClearColor(g_backgroundColor);
	g_theRenderer->Clear();

	Texture* attractScreen = g_theRenderer->CreateOrGetTexture("Data/Images/AdventureAttract.png");
	g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *attractScreen, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
}
void Game::Render_Playing()
{
	m_currentMap->Render();
}
void Game::Render_Paused()
{
	Vector2 bottomLeftOrtho = Vector2(0.f, 0.f);
	Vector2 topRightOrtho = Vector2((float)m_maps[0]->m_dimensions.x,(float)m_maps[0]->m_dimensions.y);

	g_theRenderer->SetOrtho(bottomLeftOrtho, topRightOrtho);
	g_theRenderer->ClearColor(g_backgroundColor);
	g_theRenderer->Clear();

	g_theRenderer->DrawAABB(AABB2(bottomLeftOrtho, topRightOrtho), Rgba(0.f, 0.f, 0.f, .50f));
	std::string pauseTexture;

	pauseTexture = "Data/Images/PauseScreenTransparent.png";

	Texture* pauseScreenTexture = g_theRenderer->CreateOrGetTexture(pauseTexture);
	g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *pauseScreenTexture, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
}
void Game::Render_Defeat()
{
	Vector2 bottomLeftOrtho = Vector2(0.f, 0.f);
	Vector2 topRightOrtho = Vector2((float)m_maps[0]->m_dimensions.x,(float)m_maps[0]->m_dimensions.y);

	g_theRenderer->SetOrtho(bottomLeftOrtho, topRightOrtho);
	g_theRenderer->ClearColor(g_backgroundColor);
	g_theRenderer->Clear();

	g_theRenderer->DrawAABB(AABB2(bottomLeftOrtho, topRightOrtho), Rgba(0.f, 0.f, 0.f, .50f));
	std::string pauseTexture;

	pauseTexture = "Data/Images/Dead.png";

	Texture* pauseScreenTexture = g_theRenderer->CreateOrGetTexture(pauseTexture);
	g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *pauseScreenTexture, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
}
void Game::Render_Victory()
{
	Vector2 bottomLeftOrtho = Vector2(0.f, 0.f);
	Vector2 topRightOrtho = Vector2((float)m_maps[0]->m_dimensions.x,(float)m_maps[0]->m_dimensions.y);

	g_theRenderer->DrawAABB(AABB2(bottomLeftOrtho, topRightOrtho), Rgba(0.f, 0.f, 0.f, .50f));
	std::string pauseTexture;

	pauseTexture = "Data/Images/VictoryScreenTransparent.png";

	Texture* pauseScreenTexture = g_theRenderer->CreateOrGetTexture(pauseTexture);
	g_theRenderer->DrawTexturedAABB(AABB2(bottomLeftOrtho, topRightOrtho), *pauseScreenTexture, Vector2(0, 1), Vector2(1, 0), Rgba(1.f, 1.f, 1.f, 1.f));
}


//utility methods
void Game::GenerateMap(const std::string& definitionName, const std::string& mapName)
{
	MapDefinition* definition = MapDefinition::s_definitions[definitionName];
	Map* newMap = new Map(definition, mapName);

	m_maps.push_back(newMap);
}

Map* Game::GenerateMapWithDefinition(MapDefinition* mapDefinition, const std::string& mapName)
{
	Map* newMap = new Map(mapDefinition, mapName);
	m_maps.push_back(newMap);

	return newMap;
}


//TODO: might move some of this logic out of the game and into the adventure itself. Problem is the maps are really controlled by the game. -_0_-
void Game::GenerateAdventure(const std::string& definitionName)
{
	AdventureDefinition* adventureDefinition = AdventureDefinition::s_definitions[definitionName];
	m_currentAdventure = new Adventure(adventureDefinition);

	//generate each map according to their definition
	for (std::map<std::string,AdventureMapGenerationStep*>::iterator mapIterator=adventureDefinition->m_mapsToGenerate.begin(); mapIterator!=adventureDefinition->m_mapsToGenerate.end(); ++mapIterator)
	{
		AdventureMapGenerationStep* mapStep = mapIterator->second; //store off step to make logic nicer to look at below

		MapDefinition* mapDef = mapStep->m_mapDefinition;
		std::string mapName = mapStep->m_mapName;

		Map* newMap = GenerateMapWithDefinition(mapDef, mapName);

		if(newMap->m_Name == adventureDefinition->m_startMapName)
		{
			m_currentMap = newMap;
		}

		//add portals to the maps
		for (std::map<std::string,AdventureMapPortalGenerationStep*>::iterator portalIterator=mapStep->m_mapPortals.begin(); portalIterator!=mapStep->m_mapPortals.end(); portalIterator++)
		{
			AdventureMapPortalGenerationStep* portalStep = portalIterator->second; //store off step to make logic nicer to look at below

			PortalDefinition* portalDef = portalStep->m_portalDefinition;
			Portal* newPortal = new Portal(portalDef, portalStep->m_onTileType, portalStep->m_toMap, portalStep->m_toTileType, portalStep->m_reciprocalType);
			
			newMap->SpawnPortal(newPortal);		
		}
		//TODO: Add other entities to map same as portals (actors and items)
		/*TODO: NOTE - PROBABLY NEED TO ADD A VARIABLE IN THE TILEINFO STRUCT ABOUT WHETHER OR NOT SOMETHING HAS ALREADY 
		SPAWNED THERE SO YOU DON'T PLACE PORTALS, ITEMS, AND ACTORS ON TOP OF ONE ANOTHER*/
	}

	//TODO: Will have to revisit this logic so you aren't updating portals on your HUB map every time you new off an adventure.
	//TODO: Will have to connect up the EXIT portal on the last map of the adventure to go back to the HUB world portal or Location
	for(size_t mapIndex = 0; mapIndex < m_maps.size(); mapIndex++)
	{
		for(size_t portalIndex = 0; portalIndex < m_maps[mapIndex]->m_portals.size(); portalIndex++)
		{			
			Portal* currentPortalToPair = m_maps[mapIndex]->m_portals[portalIndex];

			if(currentPortalToPair->m_reciprocalType != nullptr) //do we even need to pair a portal with another?
			{
				if(currentPortalToPair->m_destinationCoordinates == Vector2(0,0)) //have we already paired it?
				{
					bool didSuccessfullyPairPortal = false;

					for (std::vector<Map*>::iterator mapIterator= m_maps.begin(); mapIterator!=m_maps.end(); mapIterator++)
					{
						if((*mapIterator) != m_maps[mapIndex]) //make sur ewe aren't looking at ourselves.
						{
							if((*mapIterator)->m_Name == currentPortalToPair->m_destinationMapName)
							{
								PortalDefinition* portalDef = currentPortalToPair->m_reciprocalType;
								Portal* newPortal = new Portal(portalDef, currentPortalToPair->m_destinationTileType, currentPortalToPair->m_myMap->m_Name, currentPortalToPair->m_onTileType, currentPortalToPair->m_portalDef);

								(*mapIterator)->SpawnPortal(newPortal);
								currentPortalToPair->m_destinationCoordinates = newPortal->m_center;
								currentPortalToPair->m_destinationPortal = newPortal;
								currentPortalToPair->m_destinationMap = newPortal->m_myMap;

								//add data from portal we are trying to pair to destination portal
								newPortal->m_destinationCoordinates = currentPortalToPair->m_center;
								newPortal->m_destinationPortal = currentPortalToPair;
								newPortal->m_destinationMap = currentPortalToPair->m_myMap;

								didSuccessfullyPairPortal = true;
							}
						}			

						if(didSuccessfullyPairPortal)
							break;
					}
				}
			}
			/*TODO: Later on we might want to have a portal that just drops you on a location on another map as opposed to a portal.
			That logic would go in an elseif here. NOTE - Would use the ToTileType variable*/
		}
	}

	//Add player to starting map
	Vector2 playerSpawnLocation = Vector2(0,0);
	for(std::vector<Tile>::iterator tileIterator=m_currentMap->m_tiles.begin(); tileIterator!=m_currentMap->m_tiles.end(); tileIterator++)
	{
		if(tileIterator->m_tileDef->m_Name == adventureDefinition->m_startOnTileType)
		{
			playerSpawnLocation= Vector2(((float)tileIterator->m_tileCoords.x) + 0.5f, ((float)tileIterator->m_tileCoords.y) + 0.5f);
			
			break;
		}
	}
	
	m_currentMap->SpawnActorByName("Player", playerSpawnLocation);
	g_isPlayerAlive = true;
}

void Game::ChangeMap()
{
	if(g_mapChangeTarget != "default")
	{
		for (std::vector<Map*>::iterator mapIterator= m_maps.begin(); mapIterator!=m_maps.end(); mapIterator++)
		{
			if((*mapIterator)->m_Name == g_mapChangeTarget)
			{
				m_currentMap = (*mapIterator);
			}
		}

		g_mapChangeTarget = "default";
	}
}

//initializer methods
void Game::InitializeGameConfig()
{
	tinyxml2::XMLDocument gameConfigDoc;
	gameConfigDoc.LoadFile("Data/GameConfig.xml");

	tinyxml2::XMLElement* pRoot = gameConfigDoc.FirstChildElement();
	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*pRoot);	
}

void Game::InitializeFonts()
{
	g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
}

void Game::InitializeTileDefinitions()
{
	g_tileSpriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), 8, 8);
	TileDefinition::Initialize("Data/Definitions/TileDefinition.xml");
}

void Game::InitializeMapDefinitions()
{
	MapDefinition::Initialize("Data/Definitions/MapDefinition.xml");
}

void Game::InitializeActorDefinitions()
{
	ActorDefinition::Initialize("Data/Definitions/ActorDefinition.xml", *g_theRenderer);
}

void Game::InitializeItemDefinitions()
{
	ItemDefinition::Initialize("Data/Definitions/ItemDefinition.xml", *g_theRenderer);
}

void Game::InitializeProjectileDefinitions()
{
	ProjectileDefinition::Initialize("Data/Definitions/ProjectileDefinition.xml", *g_theRenderer);
}

void Game::InitializePortalDefinitions()
{
	PortalDefinition::Initialize("Data/Definitions/PortalDefinition.xml", *g_theRenderer);
}

void Game::InitializeAdventureDefinitions()
{
	AdventureDefinition::Initialize("Data/Definitions/AdventureDefinition.xml");
}
