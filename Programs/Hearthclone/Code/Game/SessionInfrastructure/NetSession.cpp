#include "Game\SessionInfrastructure\NetSession.hpp"
#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Core\EngineCommon.hpp"


//  =========================================================================================
NetSession::NetSession()
{
}

//  =========================================================================================
NetSession::~NetSession()
{
}

//  =========================================================================================
void NetSession::RegisterMessage(const char* messageId, NetMessageProcessorCallback callback)
{
	TODO("first loop and confirm we aren't trying to map a message that already has the idental id");

	NetMessageProcessorRegistration* newMessage = new NetMessageProcessorRegistration(messageId, callback);
	m_registeredMessages.push_back(newMessage);
}

//  =========================================================================================
bool NetSession::AddBinding(const int port)
{
	bool isBindSuccessful = false;

	//if our address doesn't bind our port or address is bad
	NetAddress* outAddress = new NetAddress();
	isBindSuccessful = GetLocalIP(outAddress, port);

	//if our bind call is unsuccessful the port is already bound and in use
	UDPSocket* newSocket = new UDPSocket();
	isBindSuccessful = newSocket->Bind(*outAddress);

	//if did bind is successful, add this binding to our ports
	if (isBindSuccessful)
	{
		newSocket->SetBlocking(false);
		m_openPorts.push_back(newSocket);
	}
		

	newSocket = nullptr;
	outAddress = nullptr;
	return isBindSuccessful;
}

//  =========================================================================================
void NetSession::AddConnection(NetAddress& connectionAddress)
{
	TODO("Prevent duplicates by checking against m_connections");

	NetConnection* newConnection = new NetConnection();
	newConnection->m_address = &connectionAddress;
	newConnection->m_bytePacker = new BytePacker();
	newConnection->m_connectionIndex = (int)m_connections.size() - 1;

	m_connections.push_back(newConnection);

	newConnection = nullptr;}

//  =========================================================================================
void NetSession::ProcessIncomingMessages()
{
	//for each socket that's bound, I should process each connection

	for(int openSocketIndex = 0; openSocketIndex < (int)m_openPorts.size(); ++openSocketIndex)
	{
		for (int connectionIndex = 0; connectionIndex < (int)m_connections.size(); ++connectionIndex)
		{
			byte_t buffer[PACKET_MTU];
			size_t read = m_openPorts[openSocketIndex]->ReceiveFrom(m_connections[connectionIndex]->m_address, buffer, PACKET_MTU);

			if (read > 0U)
			{
				unsigned int max_bytes = GetMinInt(read, (size_t)128);
				unsigned int string_size = max_bytes * 2U + 3U;
				char* buffer = new char[string_size];
				sprintf_s (buffer, 3U, "0x");
				byte_t *iter = (byte_t*)buffer;
				iter += 2U;

				for (unsigned int i = 0; i < read; ++i)
				{
					sprintf_s((char*)iter, 3U, "%02X", buffer[i]);
					iter += 2U;
				}
				*iter = NULL;
			}
		}
	}
	
}

//  =========================================================================================
void NetSession::ProcessOutgoingMessages()
{
	TODO("Might want to set a max similar to the 'Devconsole flush console queue");
	for (int outMessageIndex = 0; outMessageIndex < (int)m_outMessageQueue.GetSize(); ++outMessageIndex)
	{
		NetSendInfo outInfo;
		m_outMessageQueue.dequeue(&outInfo);

		m_openPorts.at(0)->SendTo(*outInfo.toAddress.m_address, outInfo.message.m_message.c_str(), outInfo.message.m_messageSize);
	}
}

