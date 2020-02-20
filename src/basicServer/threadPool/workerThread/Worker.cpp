#include "pch.h"
#include "worker.h"
#include <exceptions\Exception.h>
#include "..\..\connection\Connection.h"
#include "..\..\buffer\OverlappedBuffer.h"
#include "..\IOPool.h"
#include "..\..\SocketServer.h"
#include <basicServer\protocol\BaseMsg.h>

YWorker::YWorker(YIOCP& iocp, unsigned nThreadSN /*= 0*/)
	:YAbstractThread(nThreadSN)
	,pServer(nullptr)
	,m_iocp(iocp)
	,pDispatchCompleteEvent(nullptr)
{
}

YWorker::~YWorker()
{
}

int YWorker::Run()
{
	while (true) {
		ULONG_PTR completionKey;
		DWORD dwNumBytes;
		OVERLAPPED* pOverlapped;
		DWORD dwLastError = 0;
		if (!m_iocp.GetStatus(&completionKey, &dwNumBytes, &pOverlapped, &dwLastError)) {
			LOGFATAL("worker thread getstatus failed: %d", dwLastError);
			break;
		}else if (completionKey == 0)//stop
			break;

		pDispatchCompleteEvent->Set();		

		YConnection* pConn = reinterpret_cast<YConnection*>(completionKey);
		YOverlappedBuffer* pBuf = YOverlappedBuffer::getFromOverlapped(pOverlapped);

		DispatchType dispatchType = static_cast<DispatchType>(dwNumBytes);
		switch (dispatchType) {
		case DispatchType::connectionEstablished:
			doConnectionEstablished(pConn);
			break;
		case DispatchType::processRequest:
			doProcess(pConn, pBuf);
			break;
		default:
			throw YException("unknown dispatch type", "Worker", "Run");
		}
	}
	return THREAD_SUCCESS;
}

void YWorker::doConnectionEstablished(YConnection* pConn)
{
	LOGINFO(_T("connection established from %s:%d"), pConn->getIP().c_str(), pConn->getPort());
	pIOPool->associateDevice(reinterpret_cast<HANDLE>(pConn->getSocket()), reinterpret_cast<ULONG_PTR>(pConn));
	pIOPool->handleRead(reinterpret_cast<ULONG_PTR>(pConn));
}

void YWorker::doProcess(YConnection* pConn, YOverlappedBuffer* pBuf)
{
	pIOPool->handleRead(reinterpret_cast<ULONG_PTR>(pConn));

	YBaseMsg msg(pBuf);
	std::wcout << _T("Received msg: ") << msg.getContent()<<std::endl;
	std::string sResponse = "request ok 202";
	pConn->releaseBuffer(pBuf);

	pIOPool->handleWrite(reinterpret_cast<ULONG_PTR>(pConn), sResponse, sResponse.length());
}
