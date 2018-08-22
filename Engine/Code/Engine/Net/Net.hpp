#pragma once

//engine/internal/windowscommon.h - use this anytime you'd use windows.h
//order matters here
#define WIN32_LEAN_AND_MEAN
#include <Winsock2.h>
#include <WS2tcpip.h>  //ipv6 (optional)
#include <Windows.h>

//Engine/Net/Net.hpp
#pragma comment(lib, "ws2_32.lib") //winsock libraries

class Net
{
public:
	Net();
	~Net();

public:
	static bool Startup();
	static void Shutdown();
};



