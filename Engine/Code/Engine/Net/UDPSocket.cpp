#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"



UDPSocket::UDPSocket()
{
}


UDPSocket::~UDPSocket()
{
}

bool UDPSocket::Bind(const NetAddress& address, uint16_t portRange)
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

	address.ToSockAddr((sockaddr*)&sockAddr, &sockAddrLength);

	// try to bind - if it succeeds - great.  If not, try the next port in the range.
	int result = ::bind( mySocket, (sockaddr*)&sockAddr, (int)sockAddrLength );
	if (result == 0) 
	{
		m_socketHandle = (void*)mySocket; 
		m_address = address; 
		return true; 
	} 

	return false; 
}

size_t UDPSocket::SendTo(const NetAddress& address, const void* data, const size_t byteCount)
{
	if (IsClosed())
	{
		return false;
	}

	sockaddr_storage sockAddr;

	size_t addressLength = 0;

	address.ToSockAddr((sockaddr*)&sockAddr, &addressLength);

	SOCKET sock = (SOCKET)GetSocketHandle();

	int sent = ::sendto(sock,	//socket we're sending from
		(const char*)data,		//data we want to send
		(int)byteCount,			//bytes to send
		0,						//unused flags
		(sockaddr*)&sockAddr,	//address we're sending to;
		(int)addressLength);

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

size_t UDPSocket::ReceiveFrom(NetAddress* outAddress, const void* outBuffer, const size_t maxReadSize)
{
	if (IsClosed())
	{
		return 0U;
	}

	sockaddr_storage fromAddr; //who am i getting the data from;
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

