#include "pch.h"
#include "WorkerPool.h"

YWorkerPool::YWorkerPool(YEvent& event, size_t initActiveThreads /*= 4*/, size_t maxActiveThreads /*= 10*/, size_t minActiveThreads /*= 4*/)
	:YIOCPThreadPool<YWorker>(initActiveThreads, maxActiveThreads, minActiveThreads)
	, pServer(nullptr)
	, pIOPool(nullptr)
	, dispatchCompleteEvent(event)
{

}

void YWorkerPool::handle(ULONG_PTR pSocket, DWORD dwNumBytes, OVERLAPPED* pOverlapped)
{
	dispatch(pSocket, dwNumBytes, pOverlapped);
}

void YWorkerPool::OnInitialise(YAbstractThread* pThread)
{
	YIOCPThreadPool<YWorker>::OnInitialise(pThread);
	YWorker* pWorker = static_cast<YWorker*>(pThread);
	pWorker->setServer(pServer);
	pWorker->setIOPool(pIOPool);
	pWorker->setDispatchCompleteEvent(&dispatchCompleteEvent);
}
