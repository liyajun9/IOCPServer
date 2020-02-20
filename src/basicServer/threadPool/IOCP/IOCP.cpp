#include "pch.h"
#include "IOCP.h"
#include <exceptions\SystemException.h>
#include <log\Log.h>

YIOCP::YIOCP(size_t numMaxConcurrentThreads)
	/*:hIOCP(::CreateIoCompletionPort(NULL, NULL, 0, numMaxConcurrentThreads))*/
{
	hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numMaxConcurrentThreads);
	if (hIOCP == NULL)
		throw YSystemException("IOCP", "YIOCP");
}

YIOCP::~YIOCP()
{
	CloseHandle(hIOCP);
}

void YIOCP::AssociateDevice(HANDLE hDevice, ULONG_PTR completionKey) const
{
	if (hIOCP != ::CreateIoCompletionPort(hDevice, hIOCP, completionKey, 0))
		throw YSystemException("IOCP", "AssociateDevice");
}

void YIOCP::PostStatus(ULONG_PTR completionKey, DWORD dwNumBytes /*= 0*/, OVERLAPPED* pOverlapped /*= 0*/) const
{
	try {
		if (FALSE == ::PostQueuedCompletionStatus(hIOCP, dwNumBytes, completionKey, pOverlapped))
			throw YSystemException("IOCP", "PostStatus");
	}
	catch (std::exception & e) {
		LOGFATAL(e.what());
	}
}

bool YIOCP::GetStatus(ULONG_PTR* pCompletionKey, DWORD* pNumBytes, OVERLAPPED** ppOverlapped, DWORD * pLastError /*= NULL*/, DWORD dwMilliseconds/* = INFINITE*/) const
{
	BOOL bRes = ::GetQueuedCompletionStatus(hIOCP, pNumBytes, pCompletionKey, ppOverlapped, dwMilliseconds);
	if (pLastError)
		*pLastError = GetLastError();
	return bRes > 0;
}