#pragma once
#include "Game\Block.hpp"
#include <string.h>
#include "Engine\Math\Vector3.hpp"
#include <vector>
#include "Game\Actor.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Game\BlockDefinition.hpp"
#include "Engine\Math\IntVector3.hpp"
#include "Game\Encounter.hpp"
#include "Game\HeatMap.hpp"

class Encounter;
class Actor;

class Map
{
public:
	Map(){};
	void GenerateMap(Encounter* encounter);
	void GenerateActors(const Encounter& encounter);
	
	int GetBlockIndexByPosition(const IntVector3& position);
	int GetBlockIndexByPositionXZ(const IntVector2 & positionXZ);
	IntVector2 GetBlockPositionByIndexXZ(const int& index);

	TODO("Add block position by index function");
	//IntVector3 GetBlockPositionByIndex(int index);
	Block* GetBlock(const IntVector3& position);
	Block* SetBlock(const IntVector3& position);

	//Accessors for common map data
	int GetBlockCount() {return m_dimensions.x * m_dimensions.z * m_dimensions.y;};
	int GetBlocksXZCount(){return m_dimensions.x * m_dimensions.z;};
	int GetWidth() {return m_dimensions.x;};
	int GetDepth() {return m_dimensions.z;};
	int GetHeight() {return m_dimensions.y;};
	int GetHeightForPositionXZ(IntVector2 baseBlockPositionXZ); //xz	

	//convenience functions
	void DrawTileHighlightAtPosition(IntVector2 blockPosition, Rgba color);	

	//std::vector<Vector3> GetTraversableTiles(IntVector3 start, int speed, int jump) const;

	void Update(float deltaSeconds);
	void Render();
	
	void RegisterBlockType(std::string name, IntVector2 tileTop, IntVector2 tileSide, IntVector2 tileBottom);
	BlockDefinition* GetBlockType(std::string name);

	//pathing
	void SetActionableTilesForActor(Actor* actor,  bool doesExcludeOccupiedTiles, int maxDistance, int minDistance);
	void ResetActionableTiles();
	void UpdateIsCursorPositionInPathingIndices();
	std::vector<int> GetOrderedIndexesForShortestPathInCurrentHeatMap(IntVector2 endPosition);

	void UpdateAreaOfEffectTiles(AbilityStruct ability);
	bool GetIsPositionInsideAreaOfEffectTiles(IntVector2 position);
	void ResetAreaOfEffectTiles();

	Actor* GetActorAtCursorPosition();

public:

	std::string m_mapFilePath;
	IntVector2 m_cursorPosition = IntVector2(0,0);
	IntVector2 m_currentSelectedActorPosition = IntVector2(0,0);
	std::vector<Actor*> m_actors;
	int m_maxHeight;

	//definition information
	Texture* m_blockDefinitionTexture;
	std::vector<BlockDefinition*> m_blockDefinitions;

	//Block m_blocks[16][maxHeight][16]
	Block* m_blocks;
	IntVector3 m_dimensions; //XYZ

	std::vector<int> m_pathingDrawTilesIndices;
	HeatMap* m_heatMap = nullptr;
	std::vector<int> m_pathTiles;


	//area of effect values
	std::vector<int> m_areaOfEffectDrawTilesIndices;
	int m_areaOfEffectCenterIndex = -1;

	std::vector<int> m_heights; //costs

	bool m_isCursorPositionInPathingIndices;
	Encounter* m_encounterReference = nullptr;
};