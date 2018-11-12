#pragma once
#include <string>
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\LogSystem.hpp"

struct sockaddr; //getes around including winsock globally if you're into that sort of thing;

class NetAddress
{
public:
	NetAddress();
	~NetAddress();
	NetAddress(const char* string);
	NetAddress(const char* string, int port);
	NetAddress(const sockaddr* address);

	bool operator==( const NetAddress& compare ) const;				// vec2 == vec2

	bool ToSockAddr(const sockaddr* addr, size_t* outSize) const;
	bool FromSockAddr(const sockaddr* sa);

	std::string ToString() const;

public:
	uint m_Ipv4Address;
	uint16 m_port;

public:
	//static NetAddress GetLocal();
	static void GetMyHostAddress(sockaddr* outAddress, int* outAddrlen, const char* service = "80");
	static bool GetAddressForHost(sockaddr* outAddress, int* outAddrlen, const char* hostName, const char* service);

	/*
	optional:
	static uint GetAllForHost(NetAddress* out,
		uint maxCount,
		const char* hostName,
		const char* serviceName);
	static uint GetAllLocal(NetAddress* out, uint maxCount);
	*/
};
