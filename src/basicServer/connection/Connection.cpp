#include "pch.h"
#include "Connection.h"
#include <utils/SocketUtils.h>
#include <log\Log.h>

std::allocator<YConnection> YCachedObj<YConnection>::allocator;
const size_t YCachedObj<YConnection>::chunk = 10;

std::list<YConnection*> YCachedObj<YConnection>::freeList;
std::list<YConnection*> YCachedObj<YConnection>::inuseList;

YConnection::YConnection(size_t nSessionDataSize /*= 1*/)
	:sessionData(nSessionDataSize)
	,recvBuffer(std::unique_ptr<YOverlappedBuffer>(new YOverlappedBuffer()))
	,readSerialNum(0), writeSerialNum(0)
	,nextRead(1), nextWrite(1)
	,socket(INVALID_SOCKET)
{
}

void YConnection::clear()
{
	socket = INVALID_SOCKET;
	port = 0;
	remoteAddress = {};
	sessionData.clear();
	readSerialNum = 0;
	writeSerialNum = 0;
	nextRead = 1;
	nextWrite = 1;
	recvBuffer.reset();
	outOfOrderReadBufList.clear();
	outOfOrderWriteBufList.clear();
}

void YConnection::bind(SOCKET s, const SOCKADDR* pAddr)
{
	socket = s;
	memcpy(&remoteAddress, pAddr, sizeof(SOCKADDR));
	sIP = NS_Yutils::getIPFromSockAddr(pAddr);
	port = NS_Yutils::getPortFromSockAddr(pAddr);
}

void YConnection::gracefulClose()
{
	if (SOCKET_ERROR == ::shutdown(socket, SD_SEND)) {
		abortiveClose();
	}
	else {
		::closesocket(socket);
		clear();
	}	
}

void YConnection::abortiveClose()
{
	LOGERROR("AbortiveClose");
	LINGER linger = {};
	linger.l_onoff = 1;
	linger.l_linger = 0;
	::setsockopt(socket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&linger), sizeof(LINGER));
	::closesocket(socket);
	clear();
}

std::shared_ptr<YOverlappedBuffer> YConnection::allocatorNormalUsedBuffer()
{
	std::shared_ptr<YOverlappedBuffer> spBuf(new YOverlappedBuffer());
	return spBuf;
}

void YConnection::releaseBuffer(YOverlappedBuffer* pBuf)
{
	std::lock_guard<std::mutex> lock(mtx);
	tstring key = makeBufferKey(pBuf->getTaskType(), pBuf->getSerialNum());
	bufferMap.erase(key);
}

YOverlappedBuffer* YConnection::allocateBuffer(TaskType type)
{
	std::lock_guard<std::mutex> lock(mtx);
	std::shared_ptr<YOverlappedBuffer> spBuf(new YOverlappedBuffer());
	long serialNum = allocateSerialNum(type);
	spBuf->setTaskType(type);
	spBuf->setSerialNum(serialNum);
	bufferMap.insert(std::make_pair(makeBufferKey(type, serialNum), spBuf));
	return spBuf.get();
}

tstring YConnection::makeBufferKey(TaskType taskType, long serialNum) {
	tostringstream sstr;
	switch (taskType) {
	case TaskType::read_request:
	case TaskType::read_completed:
		sstr << _T("read") << _T(":") << serialNum;
		break;
	case TaskType::write_request:
	case TaskType::write_completed:
		sstr << _T("write") << _T(":") << serialNum;
		break;
	default:
		//static_assert(false, "this type of tasktype is invalid!");
		throw YException("this type of tasktype is invalid!", "Connection", "makeBufferKey");
	}
	return sstr.str();
}

long YConnection::allocateSerialNum(TaskType type)
{
	switch (type) {
	case TaskType::read_request:
	case TaskType::read_completed:
		return ++readSerialNum;
	case TaskType::write_request:
	case TaskType::write_completed:
		return ++writeSerialNum;
	default:
		//static_assert(false, "this type of tasktype is invalid!");
		throw YException("this type of tasktype is invalid!", "Connection", "allocateSerialNUm");
	}
}

YOverlappedBuffer* YConnection::retrieveNextReadBuffer(YOverlappedBuffer* pCurrBuf)
{
	//return currBuf if SN = next
	if (pCurrBuf->getSerialNum() == nextRead) {
		++nextRead;
		return pCurrBuf;
	}
	else if (pCurrBuf->getSerialNum() < nextRead)
		return nullptr;

	//serialNum > nextRead
	//SN != next
	//insert it
	auto it = outOfOrderReadBufList.find(pCurrBuf->getSerialNum());
	if (it == outOfOrderReadBufList.end()) {
		outOfOrderReadBufList.insert(std::make_pair(pCurrBuf->getSerialNum(), pCurrBuf));
	}
	//find next and return
	auto nextIt = outOfOrderReadBufList.find(nextRead++);
	if (nextIt == outOfOrderReadBufList.end())
		return nullptr;
	else {
		return outOfOrderReadBufList.erase(nextIt)->second;
	}
}

YOverlappedBuffer* YConnection::retrieveNextWriteBuffer(YOverlappedBuffer* pCurrBuf)
{
	//return currBuf if SN = next
	if (pCurrBuf->getSerialNum() == nextWrite) {
		++nextWrite;
		return pCurrBuf;
	}
	else if (pCurrBuf->getSerialNum() < nextWrite)
		return nullptr;

	//SN != next
	//insert it
	auto it = outOfOrderWriteBufList.find(pCurrBuf->getSerialNum());
	if (it == outOfOrderWriteBufList.end()) {
		outOfOrderWriteBufList.insert(std::make_pair(pCurrBuf->getSerialNum(), pCurrBuf));
	}
	//find next and return
	auto nextIt = outOfOrderWriteBufList.find(nextWrite++);
	if (nextIt == outOfOrderWriteBufList.end())
		return nullptr;
	else {
		return outOfOrderWriteBufList.erase(nextIt)->second;
	}
}

