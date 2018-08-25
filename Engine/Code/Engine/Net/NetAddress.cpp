#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"


//  =============================================================================
NetAddress::NetAddress()
{
}

//  =============================================================================
NetAddress::~NetAddress()
{
}

//  =============================================================================
NetAddress::NetAddress(const char* string)
{
	sockaddr* address = new sockaddr();
	int addressLength = 0;

	std::vector<std::string> strings = SplitStringOnCharacter(string, ':');

	if (strings.size() > 2)
		return;

	const char* serviceString = strings[0].c_str();
	const char* hostString = strings[1].c_str();

	if (IsStringNullOrEmpty(serviceString))
		serviceString = DEFAULT_SERVICE_PORT;

	bool success = GetAddressForHost(address, &addressLength, hostString, serviceString);

	if (success)
		FromSockAddr(address);
}

//  =============================================================================
NetAddress::NetAddress(const sockaddr* address)
{
	FromSockAddr(address);
}

//  =============================================================================
bool NetAddress::ToSockAddr(const sockaddr* addr, size_t* outSize)
{
	*outSize = sizeof(sockaddr_in);
	sockaddr_in* ipv4 = (sockaddr_in*)addr;
	memset(ipv4, 0, sizeof(sockaddr_in));

	ipv4->sin_family = AF_INET;
	ipv4->sin_addr.S_un.S_addr = m_Ipv4Address;
	ipv4->sin_port = ::htons(m_port);
	return true;
}

//  =============================================================================
bool NetAddress::FromSockAddr(const sockaddr* addr)
{
	if (addr->sa_family != AF_INET)
		return false;

	const sockaddr_in* ipv4 = (const sockaddr_in*)addr;

	uint ip = ipv4->sin_addr.S_un.S_addr;
	uint port = ::ntohs(ipv4->sin_port);

	m_Ipv4Address = ip;
	m_port = port;
	return true;
}

//  =============================================================================
const std::string NetAddress::ToString()
{
	uint8* array = (uint8*)&m_Ipv4Address;

	return Stringf("%u.%u.%u.%u:%u",
		array[0],
		array[1],
		array[2],
		array[3],
		m_port);
}

//  =============================================================================
void NetAddress::GetMyHostAddress(sockaddr* outAddress, int* outAddrlen)
{   // first, get the name of my machine
	char myName[256]; 
	if (SOCKET_ERROR == ::gethostname( myName, 256 ))
		return; 
	
	const char* service = "80"; // service is like "http" or "ftp", which translates to a port (80 or 21).  We'll just use port 80 for this example;

	// no host name - can't resolve; 
	if (IsStringNullOrEmpty(myName))
		return; 

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints; 
	memset( &hints, 0, sizeof(hints) );  // initialized to all zero; 

	hints.ai_family = AF_INET; // IPv4 Addresses
	hints.ai_socktype = SOCK_STREAM; // TCP socket (SOCK_DGRAM for UDP)
	hints.ai_flags = AI_PASSIVE; // an address we can host on; 
								 // hints.ai_flags |= FINDADDR_NUMERIC_ONLY; // will speed up this function since it won't have to lookup the address; 

	addrinfo* result = nullptr; 
	int status = getaddrinfo( myName, service, &hints, &result ); 
	if (status != 0) {
		LogSystem::GetInstance()->LogTaggedPrintf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			myName, service, ::gai_strerror(status) );
		return; 
	}

	// result now is a linked list of addresses that match our filter; 
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
			memcpy(outAddress, ipv4, sizeof(sockaddr_in));
			*outAddrlen = sizeof(sockaddr_in);
			LogSystem::GetInstance()->LogTaggedPrintf( "net", "My Address: %s", outAddress ); 
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 
}

//  =============================================================================
bool NetAddress::GetAddressForHost(sockaddr* outAddress, int* outAddrlen, const char* hostName, const char* service = "12345")
{
	// first, get the name of my machine
	if (SOCKET_ERROR == ::gethostname( (char*)hostName, 256 ))
		return false; 

//	char const *service = "80"; // service is like "http" or "ftp", which translates to a port (80 or 21).  We'll just use port 80 for this example;

								// no host name - can't resolve; 
	if (IsStringNullOrEmpty(hostName)) {
		return false; 
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

	addrinfo* result = nullptr; 
	int status = getaddrinfo( hostName, service, &hints, &result ); 
	if (status != 0) {
		LogSystem::GetInstance()->LogTaggedPrintf( "net", "Failed to find addresses for [%s:%s].  Error[%s]", 
			hostName, service, ::gai_strerror(status) );
		return false; 
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
			memcpy(outAddress, ipv4, sizeof(sockaddr_in));
			*outAddrlen = sizeof(sockaddr_in);
			found_one = true;
			break;
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

	return true;
}


