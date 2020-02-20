#pragma once
#include <utils/thread/AbstractThreadListener.h>

class YServerListener : public YAbstractThreadListener {

protected:
	virtual void OnInitialise(YAbstractThread* pThread) override;
	virtual void OnStart(YAbstractThread* pThread) override;
	virtual void OnJoin(YAbstractThread* pThread) override;
	virtual void OnWait(YAbstractThread* pThread) override;
	virtual void OnCancel(YAbstractThread* pThread) override;
	virtual void OnSuspend(YAbstractThread* pThread) override;
	virtual void OnResume(YAbstractThread* pThread) override;
	virtual void OnReturn(YAbstractThread* pThread) override;
	virtual void OnLogicError(YAbstractThread* pThread) override;
	virtual void OnException(YAbstractThread* pThread) override;
};