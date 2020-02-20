#pragma once
#include "utils\thread\AbstractThread.h"
#include "..\IOCP\IOCP.h"

class YTransferPool;
class YSocketServer;
class YConnection;
class YOverlappedBuffer;
class YIOWorker : public YAbstractThread {
public:
	explicit YIOWorker(YIOCP& iocp, unsigned nThreadSN = 0);
	~YIOWorker();

	void setServer(YSocketServer* server) { pServer = server; }
	void setTransferPool(YTransferPool* pPool) { pTransferPool = pPool; }

private:
	int Run() override;
	//void preprocess(YBaseMsg);

	void handleReadRequest(YConnection* pConn, YOverlappedBuffer* pBuff);
	void handleReadComplete(YConnection* pConn, YOverlappedBuffer* pBuff);
	void handleWriteRequest(YConnection* pConn, YOverlappedBuffer* pBuff);
	void handleWriteComplete(YConnection* pConn, YOverlappedBuffer* pBuff);

	void gracefulCloseConnection(YConnection* pConn, YOverlappedBuffer* pBuff);
	void directCloseConnection(YConnection* pConn, YOverlappedBuffer* pBuff);
	void abortiveCloseConnection(YConnection* pConn, YOverlappedBuffer* pBuff);

	YSocketServer* pServer;
	YIOCP& m_iocp;
	YTransferPool* pTransferPool;
};