#pragma once
#include <basicServer\cachedObj\cachedObj.h>
#include <WinSock2.h>
#include <map>
#include <memory>
#include <basicServer\buffer\OverlappedBuffer.h>
#include <atomic>
#include <basicServer\connection\SessionData.h>

class YConnection : public YCachedObj<YConnection>{
public:
	YConnection(size_t nSessionDataSize = 1);
	void clear();

	void bind(SOCKET s, const SOCKADDR* pAddr);
	void gracefulClose();
	void abortiveClose();

	YOverlappedBuffer* allocateBuffer(TaskType type);
	void releaseBuffer(YOverlappedBuffer* pBuf);
	std::shared_ptr<YOverlappedBuffer> allocatorNormalUsedBuffer();
	long allocateSerialNum(TaskType type);

	bool isvalid() const { return socket != INVALID_SOCKET; }
	tstring getIP() const { return sIP; }
	u_short getPort() const { return port; }
	SOCKET getSocket() const { return socket; }

	YOverlappedBuffer* retrieveNextReadBuffer(YOverlappedBuffer* pCurrBuf);
	YOverlappedBuffer* retrieveNextWriteBuffer(YOverlappedBuffer* pCurrBuf);

	//sequential IO
	std::atomic<long> readSerialNum; //start from 1
	std::atomic<long> writeSerialNum;
	std::map<long, YOverlappedBuffer*> outOfOrderReadBufList;//key = serialNum
	std::map<long, YOverlappedBuffer*> outOfOrderWriteBufList;//key = serialNum
	std::atomic<long> nextRead;
	std::atomic<long> nextWrite;

	//used to temporarily contain incompelete message
	std::unique_ptr<YOverlappedBuffer> recvBuffer;

private:
	tstring makeBufferKey(TaskType taskType, long serialNum);

	SOCKET socket;
	SOCKADDR_IN remoteAddress;
	tstring sIP;
	u_short port;
	tstring sKey; //=IP:port

	//session data
	YSessionData sessionData;

	//buffer map: key = taskType:serialNum
	std::map<tstring, std::shared_ptr<YOverlappedBuffer>> bufferMap;
};