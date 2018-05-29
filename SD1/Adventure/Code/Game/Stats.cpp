#include "Game/Stats.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"

Stats::Stats()
{
	for(int statIndex = 0; statIndex < NUM_STATS; statIndex++)
	{
		stats[statIndex] = 0;
	}
}

int Stats::GetStatIDForName(std::string statName)
{
	if(statName == "strength")
	{
		return STAT_STRENGTH;
	}
	if(statName == "armor")
	{
		return STAT_ARMOR;
	}
	if(statName == "damage")
	{
		return STAT_DAMAGE;
	}
	if(statName == "meleeRange")
	{
		return STAT_MELEE_RANGE;
	}

	return -1;
}

std::string Stats::GetNameForStatID(StatID statID)
{
	switch(statID)
	{
	case(STAT_STRENGTH):
		return "strength";
		break;
	case(STAT_ARMOR):
		return "armor";
		break;
	case(STAT_DAMAGE):
		return "damage";
		break;
	case(STAT_MELEE_RANGE):
		return "meleeRange";
		break;
	}

	return "";
}