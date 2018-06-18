#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\MathUtils.hpp"

// effects =============================================================================

//template
/*	TEMPLATE
void DoStuff(const std::map<std::string, std::string>& parameters, float deltaSeconds)
{
// Get Parameters =============================================================================
std::string thing1 = parameters.find("thing")->second;
int thing2 = atoi(parameters.find("thing2")->second.c_str());

// Process Function =============================================================================
}
*/

bool DrawEffect(Widget* targetWidget, const std::map<std::string, std::string>& parameters, float deltaSeconds)
{
	bool isEffectComplete = false;
	Stopwatch* stopwatch = GetStopwatch();

	// get parameters =============================================================================
	float totalEffectTime = ConvertStringToFloat(parameters.find("totalEffectTime")->second);
	ePlayerType playerID = (ePlayerType)ConvertStringToInt(parameters.find("playerID")->second);
	Vector2 cardStartPosition = ConvertStringToVector2(parameters.find("startPosition")->second);
	Vector2 cardFinalPosition = ConvertStringToVector2(parameters.find("endPosition")->second);
	Vector2 cardStartScale = ConvertStringToVector2(parameters.find("startScale")->second);
	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	// if first time in effect setup stopwatch =========================================================================================
	if (stopwatch == nullptr)
	{
		stopwatch = SetNewStopwatch(totalEffectTime);		
	}

	// process function =========================================================================================
	
	Vector2 position = Interpolate(cardStartPosition, cardFinalPosition, SmoothStop3((float)stopwatch->GetNormalizedElapsedTimeInSeconds()));
	Vector2 scale = Interpolate(cardStartScale, Vector2::ONE, SmoothStop3((float)stopwatch->GetNormalizedElapsedTimeInSeconds()));
	targetWidget->m_transform2D->SetLocalPosition(position);
	targetWidget->m_transform2D->SetLocalScale(scale);

	// finished logic =========================================================================================
	if (stopwatch->HasElapsed())
	{
		//lock card in position in hand once we've arrived there
		PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
		if (playerID == ENEMY_PLAYER_TYPE)
		{
			gameState->m_enemyPlayer->UpdateHandLockPositions();
			gameState->m_enemyPlayer->UpdateDeckCount();
		}		
		else if (playerID == SELF_PLAYER_TYPE)
		{
			gameState->m_player->UpdateHandLockPositions();
			gameState->m_player->UpdateDeckCount();
		}			

		Card* card = (Card*)targetWidget;

		ClearStopwatch();
		isEffectComplete = true;
	}

	// cleanup =========================================================================================
	
	stopwatch = nullptr;

	return isEffectComplete;
}

bool AttackEffect(Widget* targetWidget, const std::map<std::string, std::string>& parameters, float deltaSeconds)
{
	// Get Parameters =============================================================================
	std::string attackTarget = parameters.find("target")->second;

	// Process Function =============================================================================		

	return true;
}