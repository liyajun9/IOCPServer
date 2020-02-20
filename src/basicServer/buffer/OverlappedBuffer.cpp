#include "pch.h"
#include "OverlappedBuffer.h"

std::allocator<YOverlappedBuffer> YCachedObj<YOverlappedBuffer>::allocator;
const size_t YCachedObj<YOverlappedBuffer>::chunk = 10;

std::list<YOverlappedBuffer*> YCachedObj<YOverlappedBuffer>::freeList;
std::list<YOverlappedBuffer*> YCachedObj<YOverlappedBuffer>::inuseList;

YOverlappedBuffer::YOverlappedBuffer(size_t nSize /*= 640 * 1024*/)
	:size(nSize),serialNum(0),taskType(TaskType::noType), used(0)
	,dataVec(size)
{
	reset();
}

void YOverlappedBuffer::reset()
{
	wsaBuf.buf = &dataVec[0];
	wsaBuf.len = size;
	used = 0;
}

WSABUF* YOverlappedBuffer::getWSABuf() const
{
	return const_cast<WSABUF*>(&wsaBuf);
}

void YOverlappedBuffer::addData(const char* pData, size_t len)
{
	auto addLen = len <= size - used ? len : size - used;
	memcpy(&dataVec[used], pData, addLen);
	used += addLen;
}

void YOverlappedBuffer::setWSABUF(WSABUFMode mode)
{
	if (mode == WSABUFMode::read) {
		wsaBuf.buf = &dataVec[used];
		wsaBuf.len = size - used;
	}
	else if (mode == WSABUFMode::write) {
		wsaBuf.buf = &dataVec[0];
		wsaBuf.len = used;
	}
}

void YOverlappedBuffer::copyData(const YOverlappedBuffer* rhs)
{
	//only copy buffer
	used = rhs->used;
	wsaBuf.len = rhs->wsaBuf.len;
	dataVec = std::move(rhs->dataVec);
	wsaBuf.buf = &dataVec[0];
}

YOverlappedBuffer* YOverlappedBuffer::getFromOverlapped(OVERLAPPED* pov)
{
	if (pov) {
		ULONG_PTR pReal = reinterpret_cast<ULONG_PTR>(pov) - sizeof(ULONG_PTR); //YOverlappedBuffer继承YCachedObj，是个虚函数，在OVERLAPPED结构前将增加一个虚函数表指针，因此地址转回要减去此指针长度
		return reinterpret_cast<YOverlappedBuffer*>(pReal);
	}
	else	
		return nullptr;
}
