#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\TCPSocket.hpp"

class TCPSession
{
public:
	TCPSession();
	~TCPSession();

public:
	int m_sessionIndex = 0;
	BytePacker* m_bytePacker = nullptr;
	TCPSocket* m_socket = nullptr;
};

