#pragma once
#include "..\macros\ttype.h"
#include <winsock2.h>
#include <vector>
#include <ws2tcpip.h>

constexpr int MAX_LEN_OF_IP = 128;
constexpr int  MAX_LEN_OF_PORT = 4;
constexpr int  RADIX_DECIMAL = 10;

namespace NS_Yutils {
	void convertToWildCardSockAddr(SOCKADDR* pAddr, u_short nPort);

	void convertToSockAddr(SOCKADDR* pAddr, tstring sIP, u_short nPort);

	void _convertToSockAddr(SOCKADDR* pAddr, const tstring sIP, u_short nPort, int nProtocol, int nAddr_family, int nSock_type, int nFlags);

	inline tstring getIPFromSockAddr(const SOCKADDR* pSockaddr) {
		std::vector<TCHAR> ip(MAX_LEN_OF_IP, 0);
		InetNtop(AF_INET, reinterpret_cast<const void*>(&(reinterpret_cast<const SOCKADDR_IN*>(pSockaddr))->sin_addr), &ip[0], MAX_LEN_OF_IP);
		//inet_ntop(AF_INET, reinterpret_cast<PVOID>(&((SOCKADDR_IN*)pSockaddr)->sin_addr), &ip[0], MAX_LEN_OF_IP);
		tstring sIP(ip.begin(), ip.end());
		return sIP.c_str();
	}

	inline u_short getPortFromSockAddr(const SOCKADDR* pSockaddr) {
		return ntohs(reinterpret_cast<const SOCKADDR_IN*>(pSockaddr)->sin_port);
	}

	inline tstring makeIpPortString(const tstring& sIP, u_short nPort) {
		tostringstream sstr;
		sstr << sIP << _T(":") << nPort;
		return sstr.str();
	}
}