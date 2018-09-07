#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Time\Time.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\File\ObjectFileLoader.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\RemoteCommandService.hpp"

TheApp* g_theApp = nullptr;
TCPSocket* hostTest = nullptr;
static bool isRunning = false;


//  =============================================================================
TheApp::TheApp()
{ 
	Game::CreateInstance();
}


//  =============================================================================
TheApp::~TheApp()
{
	if(m_hostThread != nullptr)
		m_hostThread->join();
	TODO("Cleanup (delete and null out) input system and renderer");
}


//  =============================================================================
void TheApp::RunFrame()
{
	Renderer::GetInstance()->BeginFrame();	
	InputSystem::GetInstance()->BeginFrame();
	MasterClockBeginFrame();

	Update();
	PreRender();
	Render();
	PostRender();

	DebugRender::GetInstance()->EndFrame();
	InputSystem::GetInstance()->EndFrame();
	Renderer::GetInstance()->EndFrame();

	Sleep(1);
	TODO("Need to add sleep function to release CPU cycles and reduce system demand");

}


//  =============================================================================
void TheApp::Initialize()
{
	//register app commands
	CommandRegister("quit", CommandRegistration(Quit, ": Use to quit the program", "Quitting..."));
	CommandRegister("net_print_local_ip", CommandRegistration(PrintLocalIPTest, ":Use to print local ip", ""));
	CommandRegister("connect_and_send", CommandRegistration(ConnectAndSend, ":Insert IP and Port to connect with an a message you wish to send.", ""));
	CommandRegister("host_connection", CommandRegistration(HostConnectionTest, ":Insert port to listen on.", ""));
	CommandRegister("test_message", CommandRegistration(TestBytePackerSend, "Get a string to send", ""));

	//start the masterclock
	Clock* masterClock = GetMasterClock();
	masterClock->ClockSystemStartup();
	masterClock = nullptr;	

	//init mouse input settings
	InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(false);
	InputSystem::GetInstance()->GetMouse()->MouseShowCursor(true);
	InputSystem::GetInstance()->GetMouse()->SetMouseMode(MOUSE_ABSOLUTE_MODE);	

	Game::GetInstance()->Initialize();


	////bytepacker test
	//BytePacker* packer = new BytePacker(LITTLE_ENDIAN);
	//int value = 100;

	//std::string stringVal = "hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello hello";
	////std::string stringVal = "hello";
	//
	////void* data = malloc(sizeof(uint64_t));

	////packer->WriteString(stringVal.c_str());
	////packer->ReadString((char*)data, 10'000);

	////std::string testString ((char*)data);

	/*packer->WriteBytes(sizeof(int), &value);
	int outValue = 0;
	packer->ReadBytes(&outValue, sizeof(int));*/


	/*/// TESTING BYTEPACKER
	BytePacker testBP = BytePacker( LITTLE_ENDIAN );
	testBP.WriteSize( 987542100 );

	size_t readSize1;
	testBP.ReadSize( &readSize1 );

	testBP.ResetWrite();

	testBP.WriteSize( 8675309 );

	size_t readSize2;
	testBP.ReadSize( &readSize2 );

	TODO("BytePacker fails to read if I remove this line..");
	testBP.WriteString( "Hello World!" );
	char stringToRead[ 20 ];
	testBP.ReadString( stringToRead, 10 );

	testBP.WriteString( "12345678" );
	char stringToRead2[ 20 ];
	testBP.ReadString( stringToRead2, 20 );*/
}


//  =============================================================================
void TheApp::Update() 
{
	float deltaSeconds = GetMasterDeltaSeconds();
	deltaSeconds = UpdateInput(deltaSeconds);

	//ProcessHost(nullptr);	
	
	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Update(deltaSeconds);
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Update(deltaSeconds);
	}
	else
	{
		Game::GetInstance()->Update();
	}
}


//  =============================================================================
void TheApp::PreRender()
{
	Game::GetInstance()->PreRender();
}


//  =============================================================================
void TheApp::Render()
{
	//set up screen
	Game::GetInstance()->Render();

	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Render();
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Render();
	}	
}

//  =============================================================================
void TheApp::PostRender()
{
	Game::GetInstance()->PostRender();
}

//  =============================================================================
float TheApp::UpdateInput(float deltaSeconds)
{
	UNUSED(deltaSeconds); //remove if deltaSeconds needed

	if(InputSystem::GetInstance()->WasKeyJustPressed((InputSystem::GetInstance()->KEYBOARD_TILDE)))
	{
		if(!DevConsole::GetInstance()->IsOpen())
		{
			DevConsole::GetInstance()->Open();
		}
		else
		{
			DevConsole::GetInstance()->Close();
		}		
	}
	
	if (!DevConsole::GetInstance()->IsOpen())
	{
		Game::GetInstance()->UpdateInput(deltaSeconds);
	}

	if(InputSystem::GetInstance()->WasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
	{
		g_isQuitting = true;
	}

	return deltaSeconds;
}

//  =========================================================================================
bool StartTestServer(uint port)
{
	hostTest = new TCPSocket();
	bool success = hostTest->Listen(port, 16);

	if (success)
	{
		isRunning = true;
		g_theApp->m_hostThread = new std::thread(ProcessHostTest, nullptr);	

		//DevConsolePrintf("Hosting on: %s:%d", GetLocalIP(),port);
	}
		
	
	return success;
}

//  =========================================================================================
void ProcessHostTest(void*)
{
	while (isRunning)
	{
		TCPSocket* client = hostTest->AcceptConnection();
		if (client != nullptr)
		{
			ServiceClientTest(client);
		}

		delete(client);
		client = nullptr;

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

//  =========================================================================================
void CloseHostTest()
{
	hostTest->CloseConnection();

	isRunning = false;

	delete(hostTest);
	hostTest = nullptr;
}

//  =========================================================================================
void ServiceClientTest(TCPSocket* clientSocket)
{
	unsigned char buffer[256];
	int received = clientSocket->Receive(&buffer, 256 - 1U);
	if (received > 0) 
	{
		buffer[received] = NULL;
		DevConsolePrintf("Receieved: %s", buffer);

		std::string response = "gotcha";
		clientSocket->Send((void*)&response);
	}

	clientSocket->CloseConnection();
}

//  =============================================================================
//	CONSOLE COMMANDS
//  =============================================================================
void Quit(Command& cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}

//  =============================================================================
void ConnectAndSend(Command& cmd)
{
	std::string addrString = cmd.GetNextString();
	std::string messageString = cmd.GetNextString();

	if (IsStringNullOrEmpty(addrString) || IsStringNullOrEmpty(messageString))
	{
		DevConsolePrintf(Rgba::RED, "Invalid connect and send inputs! Inputs must be \"hostname:service_name\" \"msg\"");
		return;
	}

	TCPSocket socket;

	NetAddress netAddr = NetAddress(addrString.c_str());

	bool success = socket.Connect(netAddr);	

	if (!success)
	{
		DevConsolePrintf(Rgba::RED,"Could not connect to %s", addrString.c_str());
		return;
	}	
	else
	{
		DevConsolePrintf("Successfully connected to %s", netAddr.ToString().c_str());

		socket.Send((void*)&messageString);

		char payload[256];
		socket.Receive((void*)payload, 256 - 1U);
		
		DevConsolePrintf("Received: %s", std::string(payload).c_str());
	}
	
	socket.CloseConnection();
	
}

//  =============================================================================
void PrintLocalIPTest(Command& cmd)
{
	NetAddress netAddr;

	sockaddr addr;
	int addrLength = 0;
	netAddr.GetMyHostAddress(&addr, &addrLength);

	netAddr.FromSockAddr(&addr);

	std::string ip = netAddr.ToString();

	DevConsolePrintf(ip.c_str());
}



std::string GetLocalIPTest()
{
	NetAddress netAddr;

	sockaddr addr;
	int addrLength = 0;

	netAddr.GetMyHostAddress(&addr, &addrLength);

	netAddr.FromSockAddr(&addr);

	return netAddr.ToString();
}

//  =============================================================================
void HostConnectionTest(Command& cmd)
{
	int port = cmd.GetNextInt();

	if (port < 0 || port > 65535)
	{
		DevConsolePrintf(Rgba::RED, "Invalid port!");
		return;
	}

	bool success = StartTestServer(port);

	if(!success)
		DevConsolePrintf(Rgba::RED, "Could not listen on port!");
	else
	{
		std::string ip = GetLocalIPTest();
		DevConsolePrintf(Rgba::GREEN, "Hosting connection on port %s", ip.c_str());
	}
}

void TestBytePackerSend(Command& cmd)
{
	RemoteCommandService* theCommandService = RemoteCommandService::GetInstance();

	theCommandService->SendCommand(0, true, "hello");

	theCommandService = nullptr;
}


