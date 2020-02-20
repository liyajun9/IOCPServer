#pragma once
#include "IOCPThreadPool.h"
#include "workerThread/TransferWorker.h"

class YSocketServer;
class YWorkerPool;
class YTransferPool : public YIOCPThreadPool<YTransferWorker> {
public:
	explicit YTransferPool(YEvent& event, size_t dispatchTimeout = 100, size_t pullTimeoutInterval = 5000);

	void init(YSocketServer* server, YWorkerPool* pPool){	pServer = server;	pWorkerPool = pPool; }
	void handleConnectionEstablished(ULONG_PTR pConnection);
	void handleProcess(ULONG_PTR pConnection, OVERLAPPED* pOverlapped);

protected:
	void OnInitialise(YAbstractThread* pThread) override;//pass workerpool to workerthreads

private:
	YSocketServer* pServer;
	YWorkerPool* pWorkerPool;

	//dynamically control activeThread size of worker thread pool
	YEvent& dispatchCompleteEvent;
	const size_t dispatchTimeout;		//increase active thread if dispatchTimeout
	const size_t pullTimeoutInterval;	//decrease an active thread per-pullTimeoutInterval

	//support only 1 thread
	const size_t initNumThreads = 1;
	const size_t minNumThreads = 1;
	const size_t maxNumThreads = 1;
};