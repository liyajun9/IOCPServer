#pragma once

class YAbstractThread;
class YAbstractThreadListener {
public:
	virtual ~YAbstractThreadListener() = default;

protected:
	virtual void OnInitialise(YAbstractThread* pThread) {}
	virtual void OnStart(YAbstractThread* pThread) {}
	virtual void OnJoin(YAbstractThread* pThread) {}
	virtual void OnWait(YAbstractThread* pThread) {}
	virtual void OnCancel(YAbstractThread* pThread) {}
	virtual void OnSuspend(YAbstractThread* pThread) {}
	virtual void OnResume(YAbstractThread* pThread) {}
	virtual void OnReturn(YAbstractThread* pThread) {}
	virtual void OnLogicError(YAbstractThread* pThread) {}		//Run return THREAD_LOGIC_ERROR
	virtual void OnException(YAbstractThread* pThread) {}		//Run return THREAD_EXCEPTION

	friend class YAbstractThread;
};