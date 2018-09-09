#pragma once
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Net\NetAddress.hpp"

enum eSocketBlocking
{
	NON_BLOCKING,
	BLOCKING	
};

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
	size_t Send(const void* data);

	size_t Send(size_t dataSize, const void * data);

	//returns how much received
	int Receive(void* outBuffer, const size_t maxByteSize);

	bool IsClosed() const;

	bool SetBlockingState(eSocketBlocking blockingState);

public:
	static bool HasFatalError();

public:
	void* m_socketHandle = nullptr; //must cast to SOCKET

	//if listening, the address is YOUR address.
	//if you are connecting (or socket is from an accept)
	//this address is THEIR address.  (do not listen and connect on the same socket)
	NetAddress m_address;
};



