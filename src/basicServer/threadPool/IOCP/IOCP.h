#pragma once
#include <headers.h>

class YIOCP {
public:
	YIOCP(size_t numMaxConcurrentThreads);
	~YIOCP();

	void AssociateDevice(HANDLE hDevice, ULONG_PTR completionKey) const;
	void PostStatus(ULONG_PTR completionKey, DWORD dwNumBytes = 0, OVERLAPPED* pOverlapped = 0) const;
	bool GetStatus(ULONG_PTR* pCompletionKey, DWORD* pNumBytes, OVERLAPPED** ppOverlapped, DWORD* pLastError = NULL, DWORD dwMilliseconds = INFINITE) const;

private:
	HANDLE hIOCP;
};