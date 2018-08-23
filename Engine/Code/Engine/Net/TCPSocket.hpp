#pragma once
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetAddress.hpp"

class TCPSocket
{
public:
	TCPSocket();
	~TCPSocket();

	//starting stopping
	bool Listen(uint16 port, int maxQueued);
	TCPSocket* AcceptConnection();

	//for joining
	bool Connect(const NetAddress& addr);

	//when finished
	void CloseConnection();

	//returns how much sent
	size_t Send(const void* data, const size_t dataByteSize);

	//returns how much received
	size_t Receive(void* buffer, const size_t maxByteSize);

	bool IsClosed() const;

public:
	void* m_socketHandle; //must cast to SOCKET

	//if listening, the address is YOUR address.
	//if you are connecting (or socket is from an accept)
	//this address is THEIR address.  (do not listen and connect on the same socket)
	NetAddress m_address;
};

