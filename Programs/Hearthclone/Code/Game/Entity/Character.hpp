#pragma once
#include "Engine\Core\Widget.hpp"
#include <vector>
#include <string>

class Character : public Widget
{
public:
	Character();
	Character(const std::string& name) : Widget(name)
	{
		//board creation
	}
	virtual ~Character() override;

	static int GenerateNewCharacterId();
	static void ResetCharacterIndexer();
public:
	int m_characterId;
	int m_health;
	int m_attack;

	std::vector<std::string> m_tags;

	static int s_characterIndexer;
};





