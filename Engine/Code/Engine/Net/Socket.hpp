#pragma once
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\BytePacker.hpp"

enum eSocketOption : uint
{
	SOCKET_OPTION_EMPTY = 0U,
	SOCKET_OPTION_BLOCKING = BIT_FLAG(0)
};


class Socket
{
public:
	Socket();
	virtual ~Socket(); //closes

	void SetBlocking(bool blocking);

	bool Close();
	bool IsClosed() const;

	NetAddress GetAddress() const;
	void* GetSocketHandle() const;

public:
	NetAddress m_address;
	
	void* m_socketHandle = nullptr;
	eSocketOption m_options = SOCKET_OPTION_EMPTY;
};

// fatal errors cause the socket to close; 
bool IsFatalSocketError( ); 
