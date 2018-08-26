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
	return false;
}

TCPSocket* TCPSocket::AcceptConnection()
{
	return nullptr;
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
	size_t receivedData = ::recv(m_socketHandle, (char*)outBuffer, maxByteSize, 0);

	return receivedData;
}


bool TCPSocket::IsClosed() const
{

	return false;
	/*SOCKET socketPointer = (SOCKET)m_socketHandle;

	sockaddr addr = m_address.

	int peerStatus = getpeername(m_socketHandle, m_address.)*/
}