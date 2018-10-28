#pragma once
#include "Engine\Time\Clock.hpp"
#include "Engine\Renderer\ForwardRenderingPath2D.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Definitions\CardDefinition.hpp"
#include "Game\GameCommon.hpp"
#include <vector>

class NetMessage;
class NetConnection;
enum eNetMessageFlag;

enum eGameNetMessageType
{
	//start at end of other message type list
	TEST_GAME_NET_MESSAGE_TYPE = NUM_CORE_NET_MESSAGE_TYPES,

	//... more types
	UNRELAIBLE_TEST_GAME_NET_MESSAGE_TYPE = 128
};

class Game
{
public:  
	//camera members
	Camera* m_gameCamera = nullptr;

	//rendering members
	ForwardRenderingPath2D* m_forwardRenderingPath2D = nullptr;

	//clock
	Clock* m_gameClock = nullptr;

	//game members

public:
	Game();
	~Game();
	static Game* GetInstance();
	static Game* CreateInstance();

	void Initialize();

	void Update(); //use internal clock for delta seconds;
	void PreRender();
	void Render();
	void PostRender();

	void RegisterGameMessages();

	float UpdateInput(float deltaSeconds);
};

//command
void UnreliableTest(Command& cmd);

//Net message definition callbacks
bool OnUnreliableTest(NetMessage& message, NetConnection* fromConnection);
bool OnTest(NetMessage& message, NetConnection* fromConnection);





