#pragma once
#include <vector>
#include <string>
enum StatID
{
	STAT_DAMAGE,
	STAT_STRENGTH,
	STAT_ARMOR,
	STAT_MELEE_RANGE,
	NUM_STATS
};

class Stats
{
public:
	Stats();
	~Stats(){};

	int GetStatIDForName(std::string statName);
	std::string GetNameForStatID(StatID statID);
public:
	int stats[NUM_STATS];

};
