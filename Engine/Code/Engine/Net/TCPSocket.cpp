#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"

TCPSocket::TCPSocket()
{
}


TCPSocket::~TCPSocket()
{
}

bool TCPSocket::Listen(uint16 port, int maxQueued)
{
	sockaddr* addr = new sockaddr();
	int addrLength = 0;

	m_address.GetMyHostAddress(addr, &addrLength, Stringf("%i", port).c_str());
	m_address.FromSockAddr(addr);

	// now we have an address, we can try to bind it; 
	// first, we create a socket like we did before; 
	m_socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ((SOCKET)m_socketHandle == INVALID_SOCKET)
		return false;

	sockaddr_storage sockAddrStorage;
	size_t addrLen;

	bool success = m_address.ToSockAddr((sockaddr*)&sockAddrStorage, &addrLen);
	int result = ::bind(m_socketHandle, (sockaddr*)&sockAddrStorage, addrLen);

	if (result == SOCKET_ERROR) {
		// failed to bind - if you want to know why, call WSAGetLastError()
		int errorCode = WSAGetLastError();
		UNUSED(errorCode);
		::closesocket(m_socketHandle);
		return false;
	}

	// we now have a bound socket - this means we can start listening on it; 
	// This allows the socket to queue up connections - like opening a call center.
	// The number passed is NOT the max number of connections - just the max number of people who can 
	// be waiting to connect at once (think of it has a call center with N people manning the phone, but many more 
	// people who calls may be forwarded to.  Once a call is forwarded, the person answering calls can now answer a new one)
	int max_queued = 16;  // probably pick a number that is <= max number of players in your game. 
	result = ::listen(m_socketHandle, max_queued);
	if (result == SOCKET_ERROR) {
		::closesocket(m_socketHandle);
		return false;
	}

	return true;
}

TCPSocket* TCPSocket::AcceptConnection()
{
	// this will create a new socket
	// and fill in the address associated with that socket (who are you talking to?)
	sockaddr_storage theirAddr;
	int theirAddrLength = sizeof(theirAddr);

	SOCKET_T theirSocket = ::accept(m_socketHandle, (sockaddr*)&theirAddr, &theirAddrLength);

	TCPSocket* socket = new TCPSocket();
	socket->m_address.FromSockAddr((sockaddr*)&theirAddr);
	socket->m_socketHandle = theirSocket;

	return socket;
}

bool TCPSocket::Connect(const NetAddress& addr)
{
	m_socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ((SOCKET)m_socketHandle == INVALID_SOCKET)
		return false;

	sockaddr_storage sockAddrStorage;
	size_t addrLen;

	bool success = addr.ToSockAddr((sockaddr*)&sockAddrStorage, &addrLen);
	int result = ::connect(m_socketHandle, (sockaddr*)&sockAddrStorage, addrLen);	

	if (result == SOCKET_ERROR || !success)
	{
		int errorCode = WSAGetLastError();
		UNUSED(errorCode);

		std::string addrAsString = addr.ToString();
		DebuggerPrintf("Could not connect to address: %s", addrAsString);
		return false;
	}
	else
	{
		std::string addrAsString = addr.ToString();
		DebuggerPrintf("Connected to address: %s", addrAsString);
		m_address = addr;
	}
	
	return true;
}

void TCPSocket::CloseConnection()
{
	::closesocket(m_socketHandle);
}

size_t TCPSocket::Send(const void* data)
{
	std::string* message = (std::string*)data;
	int result = ::send(m_socketHandle, message->c_str(), (int)GetStringSize(*message), 0);

	if(result == SOCKET_ERROR)
		return 1;
	else
		return 0;
}

size_t TCPSocket::Receive(void* outBuffer, const size_t maxByteSize)
{
	size_t result = ::recv(m_socketHandle, (char*)outBuffer, maxByteSize, 0);

	if (result == SOCKET_ERROR)
		return -1;
	else
		return result;
}


bool TCPSocket::IsClosed() const
{

	return false;
	/*SOCKET socketPointer = (SOCKET)m_socketHandle;

	sockaddr addr = m_address.

	int peerStatus = getpeername(m_socketHandle, m_address.)*/
}