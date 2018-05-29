#pragma once
#include "Tile.hpp"
#include <vector>
#include "Entity.hpp"
#include "PlayerTank.hpp"
#include "Bullet.hpp"
#include "Turret.hpp"
#include "RaycastResult.hpp"
#include "EnemyTank.hpp"
#include "TileDefinition.hpp"
#include "Explosion.hpp"
#include "BossTank.hpp"
#include "AirStrike.hpp"

class Map
{
public:
	Map() {TileDefinition::Initialize();}
	~Map() {}
	void GenerateMap(int numTilesX, int numTilesY);
	void GenerateBossMap(int numTilesX, int numTilesY);
	void Update(float timeDelta);
	void Render();
	void IntializePlayerTank(PlayerTank* tank);
	void InitializeEnemyTurret(IntVector2 spawnTileCoordinates);
	void InitializeEnemyTank(IntVector2 spawnTileCoordinates);
	void InitializeBossTank(IntVector2 spawnTileCoordinates);
	void InstantiatePlayerBullet();
	void PlayerTankToTileCollision();
	void EnemyTankToTileCollision();
	void DetectPlayerToTileCollision(Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection);
	void DetectEntityToTileCollision(int tankIndex, Vector2 entityCenter, IntVector2 tileCoordinate, int tileDirection);
	void BulletToTileCollision(Bullet* bullet, float timeDelta);
	void PlayerTankToAirStrikeCollisions();
	void DeleteDeadEntities();
	void BulletToEntityCollision();
	void EntityToEntityCollision();
	void InstantiateEnemyBullet(float rotationDegrees, Vector2 centerOfFiringObject);
	void InstantiateExplosion(Vector2 location, float radius, float duration);
	void InstantiateAirStrike(Vector2 location, float radius, float duration);

	bool HasLineOfSight(const Vector2& startPos, const Vector2& endPos) const;
	RaycastResult Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance ) const;
public:
	std::vector<Tile> m_tiles;
	std::vector<PlayerTank*> m_playerTankEntities;
	std::vector<Bullet*> m_bulletEntities;
	std::vector<Turret*> m_enemyTurretEntities;
	std::vector<BossTank*> m_bossEntities;
	std::vector<EnemyTank*> m_enemyTankEntities;
	std::vector<Explosion*> m_explosions;
	std::vector<AirStrike*> m_airstrikes;

	int m_numTilesX;
	int m_numTilesY;
	int m_numEnemyTankSpawnOdds;
	int m_numEnemyTurretSpawnOdds;
	int m_numBosses;
	int m_solidTileType;
	int m_groundTileType;
};