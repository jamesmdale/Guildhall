#pragma once
#include "Engine\Core\Widget.hpp"
#include <vector>
#include <string>
#include "Game\Board.hpp"

class Character : public Widget
{
public:
	Character();
	Character(const std::string& name) : Widget(name)
	{
		//board creation
	}
	virtual ~Character() override;

public:
	int m_health;
	int m_attack;
	std::vector<std::string> m_tags;
	Texture* m_characterImage = nullptr;
	Board* m_boardReference = nullptr;
};

