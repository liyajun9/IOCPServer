#pragma once
#include "utils\thread\AbstractThread.h"
#include "..\IOCP\IOCP.h"
#include <utils\synchronize\Event.h>

enum class DispatchType {
	connectionEstablished = 0,
	processRequest
};

class YIOPool;
class YSocketServer;
class YConnection;
class YOverlappedBuffer;
class YWorker : public YAbstractThread {
public:
	explicit YWorker(YIOCP& iocp);
	~YWorker();

	void setServer(YSocketServer* server) { pServer = server;  }
	void setIOPool(YIOPool* pPool) { pIOPool = pPool; }
	void setDispatchCompleteEvent(YEvent* pEvent) { pDispatchCompleteEvent = pEvent;  }

private:
	int Run() override;
	void doConnectionEstablished(YConnection* pConn);
	void doProcess(YConnection* spConn, YOverlappedBuffer* pBuf);

	YSocketServer* pServer;
	YIOCP& m_iocp;
	YIOPool* pIOPool;
	YEvent* pDispatchCompleteEvent;
};