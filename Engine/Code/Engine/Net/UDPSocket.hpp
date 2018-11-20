#pragma once
#include "Engine\Net\Socket.hpp"

// class test
#define GAME_PORT 10084
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

class UDPSocket : public Socket
{
public:
	UDPSocket();
	~UDPSocket();

	bool BindToPort(int port, uint16_t portRange = 0U);
	bool Bind(NetAddress& address, uint16_t portRange = 0U);

	size_t SendTo(const NetAddress& address, const void* data, const size_t byteCount);
	size_t Receive(NetAddress* outAddress, void* outBuffer, const size_t maxReadSize);
};

