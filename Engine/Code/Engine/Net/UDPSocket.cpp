#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"


//  =============================================================================
UDPSocket::UDPSocket()
{
}

//  =============================================================================
UDPSocket::~UDPSocket()
{
	Close();
}

//  =============================================================================
bool UDPSocket::BindToPort(int port, uint16_t portRange)
{
	NetAddress address;	
	bool success = GetLocalIP(&address, (int)port);

	if (!success)
	{
		DebuggerPrintf("Address not available");
		return false;
	}

	if (!Bind(address, portRange))
	{
		DebuggerPrintf("Failed to bind.");
		return false;
	}
	else
	{
		SetBlocking(false);
		DebuggerPrintf("Socket bound: %s", GetAddress().ToString().c_str());
		return true;
	}
}

//  =============================================================================
bool UDPSocket::Bind(NetAddress& address, uint16_t portRange)
{
	// create the socket 
	SOCKET mySocket = socket( AF_INET,	// IPv4 to send...
		SOCK_DGRAM,							// ...Datagrams... 
		IPPROTO_UDP );						// ...using UDP.

	if( mySocket == INVALID_SOCKET )
		return false;

	// TODO, try to bind all ports within the range.  
	// Shown - just trying one; 
	sockaddr_storage sockAddr;
	size_t sockAddrLength;

	uint16_t attemptCount = 0;
	while (attemptCount <= portRange)
	{
		address.m_port += attemptCount;
		address.ToSockAddr((sockaddr*)&sockAddr, &sockAddrLength);

		// try to bind - if it succeeds - great.  If not, try the next port in the range.
		int result = ::bind( mySocket, (sockaddr*)&sockAddr, (int)sockAddrLength );
		if (result == 0) 
		{
			m_socketHandle = (void*)mySocket; 
			m_address = address; 
			return true; 
		} 
		else
		{
			attemptCount++;
		}
	}	
	return false; 
}

//  =============================================================================
size_t UDPSocket::SendTo(const NetAddress& address, const void* data, const size_t byteCount)
{
	if (IsClosed())
	{
		return false;
	}

	sockaddr_storage sockAddr;

	size_t addressLength = 0;

	std::string addressString = address.ToString();
	UNUSED(addressString);

	address.ToSockAddr((sockaddr*)&sockAddr, &addressLength);

	SOCKET sock = (SOCKET)GetSocketHandle();

	int sent = ::sendto(sock,	//socket we're sending from
		(const char*)data,		//data we want to send
		(int)byteCount,			//bytes to send
		0,						//unused flags
		(sockaddr*)&sockAddr,	//address we're sending to;
		(int)addressLength);

	int errorCode = WSAGetLastError();

	if (sent > 0) 
	{
		if(sent != byteCount)
			return 0; //fail case.
		else
			return (size_t)sent;
	}
	else
	{
		if ( IsFatalSocketError())
		{
			Close();
		}

		return 0;
	}
}

//  =============================================================================
size_t UDPSocket::Receive(NetAddress* outAddress, void* outBuffer, const size_t maxReadSize)
{
	if (IsClosed())
	{
		return 0U;
	}

	sockaddr_storage fromAddr; //who am I getting the data from;
	int addressLength = sizeof(sockaddr_storage);

	SOCKET sock = (SOCKET)GetSocketHandle();

	int received = ::recvfrom(sock,
		(char*)outBuffer,
		(int)maxReadSize,
		0,
		(sockaddr*)&fromAddr,
		&addressLength);

	if (received > 0)
	{
		outAddress->FromSockAddr((sockaddr*)&fromAddr);
		return received;
	}
	else
	{
		if (IsFatalSocketError())
		{
			Close();
		}

		return 0;
	}
}

