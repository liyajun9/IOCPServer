#include "pch.h"
#include "SocketServer.h"
#include <exceptions\WSAException.h>
#include <utils/SocketUtils.h>
#include <basicServer\connection\Connection.h>
#include <exceptions\SystemException.h>

YSocketServer::YSocketServer(u_short uPort, size_t initIOThreads /*= 4*/, size_t maxIOThreads /*= 10*/, size_t minIOThreads /*= 4*/
	, size_t initWorkerThreads /*= 4*/, size_t maxWorkerThreads /*= 10*/, size_t minWorkerThreads /*= 4*/)
	:port(uPort)
	,stopEvent(NULL, false, false)
	,dispatchCompleteEvent(NULL, false, false)
	,workerPool(std::make_shared<YWorkerPool>(dispatchCompleteEvent, initWorkerThreads, maxWorkerThreads, minWorkerThreads))
	,transferPool(std::make_shared<YTransferPool>(dispatchCompleteEvent))
	,ioPool(std::make_shared<YIOPool>(initIOThreads, maxIOThreads, minIOThreads))
{
	WSADATA wsaData;
	WORD wVersionRequested = 0x202;
	if (0 != ::WSAStartup(wVersionRequested, &wsaData))
		throw YSystemException("BaseServer", "YBaseServer");
}

YSocketServer::~YSocketServer()
{
	stop();
	WSACleanup();
}

void YSocketServer::init()
{
	//keep this order
	workerPool->init(this, ioPool.get());
	workerPool->start();

	transferPool->init(this, workerPool.get());
	transferPool->start();

	ioPool->init(this, transferPool.get());
	ioPool->start();

	stopEvent.Reset();
}

void YSocketServer::stop()
{
	//stop accetping
	stopEvent.Set();

	//abortive all connections
	for (auto connection : connectionMap) 
		connection.second->abortiveClose();
	connectionMap.clear();

	//stop all thread pools by order
	ioPool->stop();
	transferPool->stop();
	workerPool->stop();
}

void YSocketServer::startListening()
{
	listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
		throw YWSAException("BaseServer", "startListening-socket");

	sockAddr = {};
	NS_Yutils::convertToWildCardSockAddr(&sockAddr, port);
	if (SOCKET_ERROR == ::bind(listenSocket, &sockAddr, sizeof(SOCKADDR)))
		throw YWSAException("BaseServer", "startListening-bind");

	if(SOCKET_ERROR == ::listen(listenSocket, SOMAXCONN))
		throw YWSAException("BaseServer", "startListening-listen");
}

int YSocketServer::Run()
{
	init();
	startListening();

	while (true) {
		if (stopEvent.Wait(0))//stop
			break;

		SOCKADDR sa = {};
		int len = sizeof(SOCKADDR);
		SOCKET s = ::accept(listenSocket, &sa, &len);
		if (s == INVALID_SOCKET)
			throw YWSAException("BaseServer", "Run");

		YConnection* pConn = allocateConnection(s, &sa);

		transferPool->handleConnectionEstablished(reinterpret_cast<ULONG_PTR>(pConn));
	}
	return THREAD_SUCCESS;
}

YConnection* YSocketServer::allocateConnection(SOCKET socket, SOCKADDR* pAddr)
{
	std::shared_ptr<YConnection> spConn(new YConnection());
	spConn->bind(socket, pAddr);
	tstring key = NS_Yutils::makeIpPortString(spConn->getIP(), spConn->getPort());
	connectionMap.insert(std::make_pair(key, spConn));
	return spConn.get();
}

void YSocketServer::releaseConnection(YConnection* pConn)
{
	std::lock_guard<std::mutex> lock(mtx);
	tstring key = NS_Yutils::makeIpPortString(pConn->getIP(), pConn->getPort());
	pConn->clear();
	connectionMap.erase(key);
}

void YSocketServer::handleConnectionEstablished()
{
	std::shared_ptr<YConnection> spConn(new YConnection());
}

