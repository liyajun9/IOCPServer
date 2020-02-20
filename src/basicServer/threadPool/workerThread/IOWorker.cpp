#include "pch.h"
#include "IOWorker.h"
#include "..\..\buffer\OverlappedBuffer.h"
#include "..\..\SocketServer.h"
#include "..\..\protocol\BaseMsg.h"
#include "..\..\connection\Connection.h"

YIOWorker::YIOWorker(YIOCP& iocp)
	:YAbstractThread()
	, pServer(nullptr)
	, m_iocp(iocp)
{
}

YIOWorker::~YIOWorker()
{
}

int YIOWorker::Run()
{
	while (true) {
		DWORD dwNumBytes = 0;
		ULONG_PTR completionKey = 0;
		OVERLAPPED* pOverlapped = 0;
		DWORD dwLastError = 0;

		if (m_iocp.GetStatus(&completionKey, &dwNumBytes, &pOverlapped, &dwLastError)) {
			if (completionKey == 0)//stop
				break;
		}
		else { //fail
			if (completionKey == 0)//stop
				break;

			YConnection* pConn = reinterpret_cast<YConnection*>(completionKey);
			YOverlappedBuffer* pBuffer = YOverlappedBuffer::getFromOverlapped(pOverlapped);
			if (!pConn->isvalid())//connection has been closed
				continue;

			switch (dwLastError) {	//get status failed, abort connection!
			case ERROR_NETNAME_DELETED:
			case WSA_OPERATION_ABORTED:
			case WSAECONNRESET:
			case WSAENOBUFS:
				gracefulCloseConnection(pConn,pBuffer);
				break;
			default:
				abortiveCloseConnection(pConn,pBuffer);
				break;
			}
			continue;
		}		

		YConnection* pConn = reinterpret_cast<YConnection*>(completionKey);
		YOverlappedBuffer* pBuffer = YOverlappedBuffer::getFromOverlapped(pOverlapped);
		TaskType taskType = pBuffer->getTaskType();

		switch (taskType) {
		case TaskType::read_request:
			handleReadRequest(pConn, pBuffer);
			break;
		case TaskType::write_request:
			handleWriteRequest(pConn, pBuffer);
			break;
		case TaskType::read_completed:
			if (dwNumBytes <= 0) { //connection is closed
				gracefulCloseConnection(pConn, pBuffer);
				break;
			}
			pBuffer->use(dwNumBytes);
			handleReadComplete(pConn, pBuffer);
			break;
		case TaskType::write_completed:
			handleWriteComplete(pConn, pBuffer);
			break;
		default:
			break;
		}
	}
	return THREAD_SUCCESS;
}

void YIOWorker::handleReadRequest(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	DWORD dwFlags = 0;
	pBuff->setTaskType(TaskType::read_completed);
	pBuff->setWSABUF(YOverlappedBuffer::WSABUFMode::read);
	int nRet = ::WSARecv(pConn->getSocket(), pBuff->getWSABuf(), 1, NULL, &dwFlags,static_cast<OVERLAPPED*>(pBuff), NULL);
	DWORD dwLastError = ::GetLastError();
	if (nRet == 0) {
		gracefulCloseConnection(pConn, pBuff);
	}
	else if (nRet == SOCKET_ERROR) {
		if (dwLastError == WSAECONNRESET) {
			LOGERROR("WSAECONNRESET - WSARecv");
			directCloseConnection(pConn, pBuff);
		}
		else if (dwLastError != WSAEWOULDBLOCK && dwLastError != WSA_IO_PENDING) {
			LOGERROR("%d - WSARecv", dwLastError);
			abortiveCloseConnection(pConn, pBuff);
		}
	}
}

void YIOWorker::handleWriteRequest(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	DWORD dwFlags = 0;
	pBuff->setTaskType(TaskType::write_completed);
	pBuff->setWSABUF(YOverlappedBuffer::WSABUFMode::write);
	while (auto pNext = pConn->retrieveNextWriteBuffer(pBuff)) {
		int nRes = ::WSASend(pConn->getSocket(), pBuff->getWSABuf(), 1, NULL, dwFlags, pBuff, NULL);
		DWORD dwLastError = ::GetLastError();
		if (nRes == SOCKET_ERROR) {
			if (dwLastError == WSAENOBUFS) {
				directCloseConnection(pConn, pBuff);
				break;
			}
			else if (dwLastError != WSAEWOULDBLOCK && dwLastError != WSA_IO_PENDING) {
				abortiveCloseConnection(pConn, pBuff);
				break;
			}
		}
	}
}

void YIOWorker::handleReadComplete(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	auto tmpReceived = pConn->recvBuffer.get();//used to temporarily contain incompelete message
	auto headLen = sizeof(msgHead);

	while (auto pNext = pConn->retrieveNextReadBuffer(pBuff)) {
		auto left = pNext->getUsed();

		while (left > 0) {

			//recv msgHead if complete head hasn't been received in tmpReceived
			if (tmpReceived->getUsed() < headLen) {
				if (left <= headLen - tmpReceived->getUsed()) {
					pConn->recvBuffer->addData(pNext->getWSABuf()->buf, left);
					left = 0;
					pConn->releaseBuffer(pNext);//total buffer has been received, must release it
				}
				else {
					left -= (headLen - tmpReceived->getUsed());
					tmpReceived->addData(pNext->getWSABuf()->buf, headLen - tmpReceived->getUsed());									
				}
			}

			//recv content
			if (left > 0) {
				auto msgLen = reinterpret_cast<msgHead*>(tmpReceived->getWSABuf()->buf)->length;
				auto notReceivedMsgLen = msgLen + headLen - tmpReceived->getUsed();
				if (left < notReceivedMsgLen) {
					tmpReceived->addData(pNext->getWSABuf()->buf + (pNext->getUsed() - left), left);
					left = 0;
					pConn->releaseBuffer(pNext);//total buffer has been received, must release it
				}
				else {
					tmpReceived->addData(pNext->getWSABuf()->buf + (pNext->getUsed() - left), notReceivedMsgLen);
					left -= notReceivedMsgLen;
					//message is complete. continue to use this buffer for posting, don't release it here.
					pNext->copyData(pConn->recvBuffer.get());
					tmpReceived->reset();
					pTransferPool->handleProcess(reinterpret_cast<ULONG_PTR>(pConn), pNext);
				}
			}
			else {
				pConn->releaseBuffer(pNext);//total buffer has been received, must release it
			}
		}
	}
}

void YIOWorker::handleWriteComplete(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	pConn->releaseBuffer(pBuff);
	LOGINFO("handle WriteComplete");
}

void YIOWorker::gracefulCloseConnection(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	pConn->releaseBuffer(pBuff);
	pConn->gracefulClose();
	pServer->releaseConnection(pConn);
}

void YIOWorker::directCloseConnection(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	pConn->releaseBuffer(pBuff);
	pConn->gracefulClose();
	pServer->releaseConnection(pConn);
}

void YIOWorker::abortiveCloseConnection(YConnection* pConn, YOverlappedBuffer* pBuff)
{
	pConn->releaseBuffer(pBuff);
	pConn->abortiveClose();
	pServer->releaseConnection(pConn);
}
