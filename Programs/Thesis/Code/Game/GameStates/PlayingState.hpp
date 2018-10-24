#pragma once
#include "Game\GameStates\GameState.hpp"
#include "Game\Map\Map.hpp"
#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Core\Widget.hpp"
#include "Engine\Utility\Grid.hpp"

class PlayingState : public GameState
{
public:
	PlayingState(Camera* camera) : GameState(camera)
	{
		m_type = PLAYING_GAME_STATE;
	}

	virtual ~PlayingState() override;
	
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

	void RenderGame();
	void RenderUI();

	//bool GetInteractableWidgets(std::vector<Widget*>& outWidgets);
	//Widget* GetSelectedWidget(const std::vector<Widget*>& widgets);
	
public:

	Stopwatch* m_gameTime = nullptr;
	Widget* m_currentSelectedWidget = nullptr;

	Camera* m_uiCamera = nullptr;
	Map* m_map = nullptr;

	//optimization method flags
	bool m_isOptimized = true;
};
