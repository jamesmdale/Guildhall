#include "Engine\Renderer\Texture.hpp"

enum PauseMenuOptions
{
	RESUME,
	EXIT,
};

class PauseMenu
{
public:
	PauseMenu(){};
	void Initialize();
	void Update(float deltaSeconds);
	void Render();
	void ResetMenuState();
	float UpdateFromInput(float deltaSeconds);

public:
	PauseMenuOptions m_selectedMenuOption = RESUME;
	Texture* m_backGroundTexture;	
};