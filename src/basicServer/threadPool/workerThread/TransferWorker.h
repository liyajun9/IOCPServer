#pragma once
#include "utils\thread\AbstractThread.h"
#include "..\IOCP\IOCP.h"
#include <utils/synchronize/Event.h>

class YSocketServer;
class YWorkerPool;
class YTransferWorker : public YAbstractThread {
public:
	explicit YTransferWorker(YIOCP& iocp);
	~YTransferWorker();

	void setServer(YSocketServer* server) { pServer = server; }
	void setWorkerPool(YWorkerPool* pPool) { pWorkerPool = pPool;  }
	void setDispatchCompleteEvent(YEvent* pEvent) { pDispatchCompleteEvent = pEvent; }
	void setDispatchTimeout(size_t timeout) { dispatchTimeout = timeout;  }
	void setPullTimeoutInterval(size_t interval) { pullTimeoutInterval = interval; }

private:
	int Run() override;

	YSocketServer* pServer;
	YIOCP& m_iocp;
	YWorkerPool* pWorkerPool;
	YEvent* pDispatchCompleteEvent;

	size_t dispatchTimeout;		
	size_t pullTimeoutInterval;
};