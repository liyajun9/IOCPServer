#pragma once
#include <WinBase.h>
#include <WinSock2.h>
#include <vector>
#include "..\cachedObj\cachedObj.h"

enum class TaskType {
	noType = 0,
	connectionEstablished,
	read_request,
	read_completed,
	write_request,
	write_completed
};

class YOverlappedBuffer : public OVERLAPPED, public YCachedObj<YOverlappedBuffer> {
public:
	enum class WSABUFMode {
		read,
		write
	};

public:
	YOverlappedBuffer(size_t nSize = 640 * 1024);

	void reset();
	WSABUF* getWSABuf() const;	
	
	void use(size_t nUsed) { used += nUsed; }
	void addData(const char* pData, size_t len);
	void setWSABUF(WSABUFMode mode);

	size_t getUsed() { return used; }
	size_t getAvailable() { return size - used; }
	void setTaskType(TaskType type) { taskType = type; }
	TaskType getTaskType() { return taskType; }
	void setSerialNum(long number) { serialNum = number; }
	long getSerialNum() { return serialNum; }

	void copyData(const YOverlappedBuffer* rhs);
	static YOverlappedBuffer* getFromOverlapped(OVERLAPPED* pov);

private:
	long serialNum; //default to 0, start from 1
	TaskType taskType;
	const size_t size;
	std::vector<char> dataVec;
	size_t used;
	WSABUF wsaBuf;	
};