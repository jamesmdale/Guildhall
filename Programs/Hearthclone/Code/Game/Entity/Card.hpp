#pragma once
#include "Game\Definitions\CardDefinition.hpp"
#include "Engine\Renderer\Texture.hpp"
#include <vector>
#include <string>
#include "Engine\Core\Widget.hpp"
#include "Engine\Math\Vector2.hpp"

class Card : public Widget
{
public:
	Card();
	Card(const std::string& name) : Widget(name)
	{
		//board creation
	}
	Card(const CardDefinition* definition);
	virtual ~Card() override;

	virtual void Initialize() override;	
	void RefreshCardRenderables();
	Vector2 GetCardDimensions();

public:
	const CardDefinition* m_definition = nullptr;
	Texture* m_cardImage = nullptr;
	Texture* m_cardLayoutImage = nullptr;
	
	int m_cost;
	int m_attack;
	int m_health;

	std::vector<std::string> m_tags;
	std::string m_text;
	
	bool m_isPositionLocked = true;
	bool m_isRendering = false;
	Vector2 m_lockPosition;	//position in space the card should snap to if not being manipulated by user input
};




