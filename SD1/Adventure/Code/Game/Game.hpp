#pragma once

#include "Game\GameCommon.hpp"
#include <string>
#include <vector>
#include "Game/Map.hpp"
#include "Game/Adventure.hpp"

class Game
{
public: 	
	Game();
	~Game();

	std::vector<Map*> m_maps;
	Adventure* m_currentAdventure = nullptr;
	Map* m_currentMap = nullptr;

	void Update(float TimeDelta);
	void Render();
	void Initialize();

	//update methods
	void Update_Attract();
	void Update_Playing(float timeDelta);
	void Update_Paused(float timeDelta);
	void Update_Defeat(float timeDelta);
	void Update_Victory(float timeDelta);

	//render methods
	void Render_Attract();
	void Render_Playing();
	void Render_Paused();
	void Render_Defeat();
	void Render_Victory();
	
	void InitializeGameConfig();
	void InitializeFonts();
	void InitializeTileDefinitions();	
	void InitializeMapDefinitions();
	void InitializeItemDefinitions();
	void InitializeActorDefinitions();
	void InitializeProjectileDefinitions();
	void InitializePortalDefinitions();
	void InitializeAdventureDefinitions();

	void GenerateMap(const std::string& definitionName, const std::string& mapName);
	Map* GenerateMapWithDefinition(MapDefinition* mapDefinition, const std::string& mapName);
	void GenerateAdventure(const std::string& definitionName);
	void ChangeMap();

	void BeginFrame();
	void EndFrame();
};

extern Game* g_Game;
extern GameState g_currentState;
extern GameState g_transitionState;
extern float g_secondsInState;
extern float g_secondsTransistioning;






