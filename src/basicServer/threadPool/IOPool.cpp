#include "pch.h"
#include "IOPool.h"
#include <basicServer\buffer\OverlappedBuffer.h>
#include <basicServer\connection\Connection.h>

YIOPool::YIOPool(size_t initActiveThreads /*= 4*/, size_t maxActiveThreads /*= 10*/, size_t minActiveThreads /*= 4*/)
	:YIOCPThreadPool<YIOWorker>(initActiveThreads, maxActiveThreads, minActiveThreads)
	, pServer(nullptr)
	, pTransferPool(nullptr)
{
}

void YIOPool::associateSocket(HANDLE hSocket, ULONG_PTR pSocket)
{
	associateDevice(hSocket, pSocket);
}

void YIOPool::handleRead(ULONG_PTR pConnection)
{
	YConnection* pConn = reinterpret_cast<YConnection*>(pConnection);
	YOverlappedBuffer* pBuf = pConn->allocateBuffer(TaskType::read_request);
	dispatch(pConnection, 0, pBuf);
}

void YIOPool::handleWrite(ULONG_PTR pConnection, const std::string& sRes, size_t numBytes)
{
	YConnection* pConn = reinterpret_cast<YConnection*>(pConnection);
	YOverlappedBuffer* pBuf = pConn->allocateBuffer(TaskType::write_request);
	pBuf->addData(sRes.c_str(), numBytes);
	dispatch(pConnection, 0, pBuf);
}

void YIOPool::OnInitialise(YAbstractThread* pThread)
{
	YIOCPThreadPool<YIOWorker>::OnInitialise(pThread);
	YIOWorker* pIOWorker = static_cast<YIOWorker*>(pThread);
	pIOWorker->setServer(pServer);
	pIOWorker->setTransferPool(pTransferPool);
}
