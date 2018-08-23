#pragma once
#include "Engine\Core\WindowsCommon.hpp"

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






