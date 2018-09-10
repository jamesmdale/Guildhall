#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Net\Socket.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\BitHelper.hpp"

//  =============================================================================
Socket::Socket()
{
}

//  =============================================================================
Socket::~Socket()
{
}

//  =============================================================================
void Socket::SetBlocking(bool blocking)
{
	if(blocking)
		EnableBits((uint)m_options, (uint)SOCKET_OPTION_BLOCKING);
	else
		DisableBits((uint)m_options, (uint)SOCKET_OPTION_BLOCKING);

	u_long non_blocking = blocking ? 0 : 1;
	int success = ::ioctlsocket( (SOCKET)m_socketHandle, FIONBIO, &non_blocking );
	UNUSED(success);
}

//  =============================================================================
bool Socket::Close()
{
	int result = ::closesocket((SOCKET)m_socketHandle);

	if (result == SOCKET_ERROR)
		return false;
	else
		return true;
}

//  =============================================================================
bool Socket::IsClosed() const
{
	return false;
}

//  =============================================================================
NetAddress Socket::GetAddress() const
{
	return m_address;
}

void* Socket::GetSocketHandle() const
{
	return m_socketHandle;
}

//  =============================================================================
bool IsFatalSocketError()
{
	int errorCode = WSAGetLastError();

	if(errorCode == WSAEWOULDBLOCK || errorCode == WSAEMSGSIZE || errorCode == WSAECONNRESET || errorCode == 0)
		return false;

	return true;
}
