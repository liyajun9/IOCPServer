#include "pch.h"
#include "Event.h"
#include "..\..\exceptions\SystemException.h"

YEvent::YEvent( LPSECURITY_ATTRIBUTES lpSecurityAttributes, bool bManualReset, bool initState, const tstring& sName/*  = _T("")*/)
{
	if((m_hEvent = CreateEvent(lpSecurityAttributes, bManualReset, initState, sName.c_str())) == INVALID_HANDLE_VALUE)
		throw YSystemException("Event", "YEvent");
}

YEvent::~YEvent()
{
	CloseHandle(m_hEvent);
}

HANDLE YEvent::GetEvent() const
{
	return m_hEvent;
}

DWORD YEvent::Wait() const
{
	return WaitForSingleObject(m_hEvent, INFINITE);
}
bool YEvent::Wait(DWORD timeoutMillis) const
{
	DWORD dwRes = WaitForSingleObject(m_hEvent, timeoutMillis);
	switch(dwRes){
		case WAIT_OBJECT_0:
			return true;
		case WAIT_TIMEOUT:
			return false;
		case WAIT_ABANDONED:
		case WAIT_FAILED:
		default:
			throw YSystemException("Event", "Wait");
		return false;
	}
}

void YEvent::Set()
{
	if (!SetEvent(m_hEvent))
		throw YSystemException("Event", "Set");
}

void YEvent::Reset()
{
	if(!ResetEvent(m_hEvent))
		throw YSystemException("Event", "Reset");
}

void YEvent::Pulse()
{
	if(!PulseEvent(m_hEvent))
		throw YSystemException("Event", "Pulse");
}