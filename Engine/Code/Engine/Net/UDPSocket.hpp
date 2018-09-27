#pragma once
#include "Engine\Net\Socket.hpp"

class UDPSocket : public Socket
{
public:
	UDPSocket();
	~UDPSocket();

	bool Bind(NetAddress& address, uint16_t portRange = 0U);

	size_t SendTo(const NetAddress& address, const void* data, const size_t byteCount);
	size_t ReceiveFrom(NetAddress* outAddress, const void* buffer, const size_t maxReadSize);
};

