#include "pch.h"
#include "SocketUtils.h"
#include "encodings/CharEncodings.h"
#include "..\exceptions\WSAException.h"

void NS_Yutils::convertToWildCardSockAddr(SOCKADDR* pAddr, u_short nPort)
{
	_convertToSockAddr(pAddr, _T(""), nPort, 0, AF_INET, 0, AI_PASSIVE | AI_NUMERICSERV);
}

void NS_Yutils::convertToSockAddr(SOCKADDR* pAddr, tstring sIP, u_short nPort)
{
	_convertToSockAddr(pAddr, sIP, nPort, 0, AF_INET, 0, AI_NUMERICSERV);
}

void NS_Yutils::_convertToSockAddr(SOCKADDR* pAddr, const tstring sIP, u_short nPort, int nProtocol, int nAddr_family, int nSock_type, int nFlags)
{
	std::string sTargetIP;
	bool bWildCardAddr = sIP.empty();
	if (!bWildCardAddr)
#if defined(_UNICODE)||defined(UNICODE)
		sTargetIP = WCharToMB(sIP);
#else
		sTargetIP = sIP;
#endif

	char pszPort[MAX_LEN_OF_PORT + 1];
	memset(pszPort, 0, MAX_LEN_OF_PORT + 1);
	_itoa_s(nPort, pszPort, MAX_LEN_OF_PORT + 1, RADIX_DECIMAL);

	addrinfo* pResultAddr = NULL;
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_protocol = nProtocol;
	hints.ai_family = nAddr_family;
	hints.ai_socktype = nSock_type;
	hints.ai_flags = nFlags;

	if (bWildCardAddr) {
		if (NO_ERROR != getaddrinfo(NULL, pszPort, &hints, &pResultAddr))
			throw YWSAException("SocketUtils", "convertToSockAddr");
	}
	else {
		if (NO_ERROR != getaddrinfo(sTargetIP.c_str(), pszPort, &hints, &pResultAddr))
			throw YWSAException("SocketUtils", "convertToSockAddr");
	}

	pAddr->sa_family = pResultAddr->ai_addr->sa_family;
	memcpy(pAddr->sa_data, pResultAddr->ai_addr->sa_data, sizeof(pResultAddr->ai_addr->sa_data));
	freeaddrinfo(pResultAddr);
}
