#include "Engine\Net\TCPSession.hpp"

TCPSession::TCPSession()
{
}


TCPSession::~TCPSession()
{
	delete(m_socket);
	m_socket = nullptr;

	delete(m_bytePacker);
	m_bytePacker = nullptr;
}
