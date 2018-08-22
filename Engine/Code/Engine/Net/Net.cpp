#include "Engine\Net\Net.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

Net::Net()
{
}


Net::~Net()
{
}

bool Net::Startup()
{
	//combines two byte things into a short thing
	WORD version = MAKEWORD(2,2);
	WSADATA data;
	int error = ::WSAStartup(version, &data);

	GUARANTEE_OR_DIE(error == 0, "WINSOCK CONNECTION ERROR!");

	return(error == 0);
}

void Net::Shutdown()
{
	::WSACleanup();
}