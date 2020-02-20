#pragma once
#include "windows.h"

class YCSLocker
{
public:
	explicit YCSLocker(CRITICAL_SECTION& cs)
	: m_cs(cs){
		::EnterCriticalSection(&m_cs);
	}

	~YCSLocker(){
		::LeaveCriticalSection(&m_cs);
	}

private:
	YCSLocker() = delete;
	CRITICAL_SECTION& m_cs;
};