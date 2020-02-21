#pragma once
#include <utils/thread/abstractThread.h>
#include <utils/thread/AbstractThreadListener.h>
#include <basicServer\threadPool\IOPool.h>
#include <basicServer\threadPool\TransferPool.h>
#include <basicServer\threadPool\WorkerPool.h>
#include <map>
#include "connection/Connection.h"

class YSocketServer : public YAbstractThread {
public:
	YSocketServer(u_short uPort, size_t initIOThreads = 4, size_t maxIOThreads = 10, size_t minIOThreads = 4
		, size_t initWorkerThreads = 4, size_t maxWorkerThreads = 10, size_t minWorkerThreads = 4);

	virtual ~YSocketServer();

	int Run() override;
	void stop();

	YConnection* allocateConnection(SOCKET socket, SOCKADDR* pAddr);
	void releaseConnection(YConnection* pConn);

private:
	void init();
	void startListening();
	void handleConnectionEstablished();	

	SOCKET listenSocket;
	u_short port;
	SOCKADDR sockAddr;

	YEvent stopEvent;
	YEvent dispatchCompleteEvent;//used by transferPool to dynamically control number of threads in workerPool

	std::map<tstring, std::shared_ptr<YConnection>> connectionMap; //key = ip:port
	std::mutex mtx;

	std::shared_ptr<YIOPool> ioPool;
	std::shared_ptr<YTransferPool> transferPool;
	std::shared_ptr<YWorkerPool> workerPool;
};