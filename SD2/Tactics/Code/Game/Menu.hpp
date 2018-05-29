#include "Engine\Renderer\Texture.hpp"


enum MenuOptions
{
	PLAY,
	QUIT,
	NUM_MENU_OPTIONS
};

class Menu
{
public:
	Menu(){};
	void Initialize();
	void Update(float deltaSeconds);
	void Render();
	void ResetMenuState();
	float UpdateFromInput(float deltaSeconds);

public:
	MenuOptions m_selectedMenuOption = PLAY;
	Texture* m_backGroundTexture;	
};