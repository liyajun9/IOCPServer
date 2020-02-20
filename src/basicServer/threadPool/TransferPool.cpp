#include "pch.h"
#include "TransferPool.h"
#include "WorkerPool.h"

YTransferPool::YTransferPool(YEvent& event, size_t timeout /*= 100*/, size_t interval /*= 5000*/)
	:YIOCPThreadPool<YTransferWorker>(1, 1, 1)
	, dispatchTimeout(timeout)
	, pullTimeoutInterval(interval)
	,pWorkerPool(nullptr)
	,dispatchCompleteEvent(event)
{
}

void YTransferPool::handleConnectionEstablished(ULONG_PTR pConnection)
{
	dispatch(pConnection, static_cast<DWORD>(DispatchType::connectionEstablished), NULL);
}

void YTransferPool::handleProcess(ULONG_PTR pConnection, OVERLAPPED* pOverlapped)
{
	dispatch(pConnection, static_cast<DWORD>(DispatchType::processRequest), pOverlapped);
}

void YTransferPool::OnInitialise(YAbstractThread* pThread)
{
	YIOCPThreadPool<YTransferWorker>::OnInitialise(pThread);
	YTransferWorker* pTransferWorker = static_cast<YTransferWorker*>(pThread);
	pTransferWorker->setServer(pServer);
	pTransferWorker->setWorkerPool(pWorkerPool);
	pTransferWorker->setDispatchTimeout(dispatchTimeout);
	pTransferWorker->setPullTimeoutInterval(pullTimeoutInterval);
	pTransferWorker->setDispatchCompleteEvent(&dispatchCompleteEvent);
}


