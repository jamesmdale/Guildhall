#include "Engine\Net\Net.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

//  =============================================================================
Net::Net()
{
}

//  =============================================================================
Net::~Net()
{
}

//  =============================================================================
bool Net::Startup()
{
	//combines two byte things into a short thing
	WORD version = MAKEWORD(2,2);

	// initialize the (W)in(S)ock(A)PI.  
	// data will store information about your system (and some limitations)
	// We only use a few sockets in this class, so you can 
	// ignore this.

	WSADATA data;
	int error = ::WSAStartup(version, &data);


	//return whether we successful startup
	GUARANTEE_OR_DIE(error == 0, "WINSOCK CONNECTION ERROR! NETWORK STARTUP FAILED!");

	return(error == 0);
}

//  =============================================================================
void Net::Shutdown()
{
	::WSACleanup();
}
