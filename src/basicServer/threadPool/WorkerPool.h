#pragma once
#include "IOCPThreadPool.h"
#include "workerThread/Worker.h"
#include <basicServer\threadPool\IOPool.h>

class YWorkerPool : public YIOCPThreadPool<YWorker> {
public:
	explicit YWorkerPool(YEvent& event, size_t initActiveThreads = 4, size_t maxActiveThreads = 10, size_t minActiveThreads = 4);

	void init(YSocketServer* server, YIOPool* pPool) { pServer = server; pIOPool = pPool; }
	void handle(ULONG_PTR pSocket, DWORD dwNumBytes, OVERLAPPED* pOverlapped);

protected:
	void OnInitialise(YAbstractThread* pThread) override;//pass IOPool to workerthreads

private:
	YSocketServer* pServer;
	YIOPool* pIOPool;
	YEvent& dispatchCompleteEvent;
};