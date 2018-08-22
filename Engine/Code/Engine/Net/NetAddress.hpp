#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Winsock2.h>
#include <WS2tcpip.h>  //ipv6 (optional)
#include <Windows.h>

#include <string>
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\LogSystem.hpp"


class NetAddress
{
public:
	NetAddress();
	~NetAddress();
	NetAddress(const char* string);

	bool ToSockAddr(const sockaddr* addr);
	bool FromSockAddr(const sockaddr* sa);

	const std::string ToString();

public:
	uint ip4Address;
	uint16 port;

public:
	static NetAddress GetLocal();
	/*
	optional:
	static uint GetAllForHost(NetAddress* out,
		uint maxCount,
		const char* hostName,
		const char* serviceName);
	static uint GetAllLocal(NetAddress* out, uint maxCount);
	*/
};

struct sockaddr; //getes around including winsock globally if you're into that sort of thing;

bool GetAddressForHost(sockaddr* out, int* out_addrlen, const char* hostName, const char* service = "12345")
{
	// first, get the name of my machine
	char my_name[256]; 
	if (SOCKET_ERROR == ::gethostname( my_name, 256 )) {
		return; 
	}

	char const *service = "80"; // service is like "http" or "ftp", which translates to a port (80 or 21).  We'll just use port 80 for this example;

								// no host name - can't resolve; 
	if (IsStringNullOrEmpty(my_name)) {
		return; 
	}

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints; 
	memset( &hints, 0, sizeof(hints) );  // initialized to all zero; 

	hints.ai_family = AF_INET; // IPv4 Addresses
	hints.ai_socktype = SOCK_STREAM; // TCP socket (SOCK_DGRAM for UDP)
	//hints.ai_flags = AI_PASSIVE; // an address we can host on; 
	// hints.ai_flags |= FINDADDR_NUMERIC_ONLY; // will speed up this function since it won't have to lookup the address; 

	addrinfo *result = nullptr; 
	int status = getaddrinfo( my_name, service, &hints, &result ); 
	if (status != 0) {
		LogSystem::GetInstance()->LogTaggedPrintf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			my_name, service, ::gai_strerror(status) );
		return; 
	}

	// result now is a linked list of addresses that match our filter; 
	bool found_one = false;
	addrinfo* iter = result;
	while (iter != nullptr) {
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		
		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			// we have an address - print it!

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			memcpy(out, ipv4, sizeof(sockaddr_in));
			*out_addrlen = sizeof(sockaddr_in);
			found_one = true;
			break;
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 
}

void GetAddressExample()
{   // first, get the name of my machine
	char my_name[256]; 
	if (SOCKET_ERROR == ::gethostname( my_name, 256 )) {
		return; 
	}

	char const *service = "80"; // service is like "http" or "ftp", which translates to a port (80 or 21).  We'll just use port 80 for this example;

								// no host name - can't resolve; 
	if (IsStringNullOrEmpty(my_name)) {
		return; 
	}

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints; 
	memset( &hints, 0, sizeof(hints) );  // initialized to all zero; 

	hints.ai_family = AF_INET; // IPv4 Addresses
	hints.ai_socktype = SOCK_STREAM; // TCP socket (SOCK_DGRAM for UDP)
	hints.ai_flags = AI_PASSIVE; // an address we can host on; 
								 // hints.ai_flags |= FINDADDR_NUMERIC_ONLY; // will speed up this function since it won't have to lookup the address; 

	addrinfo *result = nullptr; 
	int status = getaddrinfo( my_name, service, &hints, &result ); 
	if (status != 0) {
		LogSystem::GetInstance()->LogTaggedPrintf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			my_name, service, ::gai_strerror(status) );
		return; 
	}

	// result now is a linked list of addresses that match our filter; 
	addrinfo *iter = result;
	while (iter != nullptr) {
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			// we have an address - print it!

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char out[256]; 
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 ); 
			LogSystem::GetInstance()->LogTaggedPrintf( "net", "My Address: %s", out ); 
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

}
