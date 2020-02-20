#pragma once
#include "IOCPThreadPool.h"
#include "workerThread/IOWorker.h"
#include "..\threadPool\TransferPool.h"

class YIOPool : public YIOCPThreadPool<YIOWorker> {
public:
	explicit YIOPool(size_t initActiveThreads = 4, size_t maxActiveThreads = 10, size_t minActiveThreads = 4);

	void init(YSocketServer* server, YTransferPool* pPool) { pServer = server; pTransferPool = pPool; }
	void associateSocket(HANDLE hSocket, ULONG_PTR pSocket);
	void handleRead(ULONG_PTR pConnection);
	void handleWrite(ULONG_PTR pConnection, const std::string& sRes, size_t numBytes);

protected:
	void OnInitialise(YAbstractThread* pThread) override;//pass transferPool to workerthreads

private:
	YSocketServer* pServer;
	YTransferPool* pTransferPool;
};