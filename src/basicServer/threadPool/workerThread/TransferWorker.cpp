#include "pch.h"
#include "TransferWorker.h"
#include <exceptions\Exception.h>
#include "..\WorkerPool.h"
#include <exceptions\SystemException.h>

YTransferWorker::YTransferWorker(YIOCP& iocp, unsigned nThreadSN /*= 0*/)
	:YAbstractThread(nThreadSN)
	,pServer(nullptr)
	,m_iocp(iocp)
	,pDispatchCompleteEvent(nullptr)
	,dispatchTimeout(0)
	,pullTimeoutInterval(0)
{
}

YTransferWorker::~YTransferWorker()
{
}

int YTransferWorker::Run()
{
	while (true) {
		static auto tLast = std::chrono::steady_clock::now();
		ULONG_PTR completionKey = 0;
		DWORD dwNumBytes = 0;
		OVERLAPPED* pOverlapped = 0;
		DWORD dwLastError = 0;
		if (!m_iocp.GetStatus(&completionKey, &dwNumBytes, &pOverlapped, &dwLastError)) {
			LOGFATAL("transferWorker thread getstatus failed: %d", dwLastError);
			break;
		}
		else if (completionKey == 0)//stop
			break;

		auto interval = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - tLast).count();
		if (interval > pullTimeoutInterval) {
			pWorkerPool->ReduceActiveThread(static_cast<int>(interval) / static_cast<int>(pullTimeoutInterval));
		}
		tLast = std::chrono::steady_clock::now();

		pDispatchCompleteEvent->Reset();
		auto t1 = std::chrono::steady_clock::now();
		pWorkerPool->handle(completionKey, dwNumBytes, pOverlapped);//transfer to worker pool
		auto dwRes = pDispatchCompleteEvent->Wait();

		auto t2 = std::chrono::steady_clock::now();			
		auto ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		if (ms > dispatchTimeout) {
			pWorkerPool->IncreaseActiveThread();
		}
	}
	return THREAD_SUCCESS;
}
