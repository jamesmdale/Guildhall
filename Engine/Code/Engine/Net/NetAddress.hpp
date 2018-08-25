#pragma once
#include <string>
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\LogSystem.hpp"


struct sockaddr; //getes around including winsock globally if you're into that sort of thing;


constexpr char* DEFAULT_SERVICE_PORT = "12345";

class NetAddress
{
public:
	NetAddress();
	~NetAddress();
	NetAddress(const char* string);
	NetAddress(const sockaddr* address);

	bool ToSockAddr(const sockaddr* addr, size_t* outSize);
	bool FromSockAddr(const sockaddr* sa);

	const std::string ToString();

public:
	uint m_Ipv4Address;
	uint16 m_port;

public:
	//static NetAddress GetLocal();
	static void GetMyHostAddress(sockaddr* outAddress, int* outAddrlen);
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
