#pragma once
#include <string>
#include "Engine/Math/IntVector2.hpp"
#include "Game/MapDefinition.hpp"
#include <vector>
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Projectile.hpp"
#include "Game/Portal.hpp"
#include "Game/Item.hpp"
#include "Game/Camera.hpp"

class Map
{
public:
	explicit Map(MapDefinition* definition, const std::string& mapName);
	void SetMapType(MapDefinition* newMapDefintion){ m_mapDef = newMapDefintion;}
	IntVector2 GetDimensions(){ return m_dimensions;}

	void Render();
	void Update(float timeDelta);

	//spawn functions
	void SpawnActorByName(std::string actorType, Vector2 spawnPosition);
	void SpawnItemByName(std::string itemType, Vector2 spawnPosition);
	void SpawnItemByDefinition(ItemDefinition* itemType, Vector2 spawnPosition);
	void SpawnActorByDefinition(ActorDefinition* actorType, Vector2 spawnPosition);
	void SpawnProjectileFromActor(Actor* spawningActor, std::string projectileType);
	void SpawnPortal(Portal* spawnPortal);

	//Collision detections
	void ProjectileToActorCollision();
	void ActorToTileCollision();
	void DetectActorToTileCollision(int entityIndex, Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection);
	void ActorToPortalCollision();
	void ActorToActorCollision();
	void ActorToItemCollision();
	void ActorToActorAttackCollision();

	//Cleanup functions
	void DeleteDeadEntities();

	//General functions
	void UpdatePlayerInput();
	int GetActorIndex();



	//void GenerateMap(const std::string& definitionName, const std::string& mapName);

public:
	std::string m_Name;
	IntVector2 m_dimensions;
	MapDefinition* m_mapDef = nullptr;
	std::vector<Tile> m_tiles;
	std::vector<Actor*> m_actors;
	std::vector<Projectile*> m_projectiles;
	std::vector<Portal*> m_portals;
	std::vector<Item*> m_items;
	Camera* m_camera;
private:
	bool m_isFullMapView = false;
	//TODO: Might make a list of entities (actors, portals, portals, etc)
	//std::vector<Entity*> m_entities;
};