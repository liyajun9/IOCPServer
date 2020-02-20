#pragma once
#include <concrt.h>
#include "..\..\Macros\ttype.h"

class YEvent{
public:
	YEvent( LPSECURITY_ATTRIBUTES lpSecurityAttributes, bool bManualReset, bool initState, const tstring& sName = _T(""));
	virtual ~YEvent();

	HANDLE GetEvent() const;
	DWORD Wait() const;
	bool Wait(DWORD timeoutMillis) const;
	void Set();
	void Reset();
	void Pulse();

private:
	HANDLE m_hEvent;
};