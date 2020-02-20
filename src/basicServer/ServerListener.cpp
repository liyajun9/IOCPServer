#include "pch.h"
#include "ServerListener.h"
#include <log\Log.h>
#include <utils/thread/AbstractThread.h>

void YServerListener::OnInitialise(YAbstractThread* pThread)
{
	LOGINFO("Server initialised");
}

void YServerListener::OnStart(YAbstractThread* pThread)
{
	LOGINFO("Server started");
}

void YServerListener::OnJoin(YAbstractThread* pThread)
{
	LOGINFO("Server joined");
}

void YServerListener::OnWait(YAbstractThread* pThread)
{
	LOGINFO("Server waited");
}

void YServerListener::OnCancel(YAbstractThread* pThread)
{
	LOGINFO("Server canceled");
}

void YServerListener::OnSuspend(YAbstractThread* pThread)
{
	LOGINFO("Server suspended");
}

void YServerListener::OnResume(YAbstractThread* pThread)
{
	LOGINFO("Server resumed");
}

void YServerListener::OnReturn(YAbstractThread* pThread)
{
	LOGINFO("Server returned");
}

void YServerListener::OnLogicError(YAbstractThread* pThread)
{
	LOGINFO("Server returned with logicError");
}

void YServerListener::OnException(YAbstractThread* pThread)
{
	LOGINFO("Server returned with exception");
	LOGERROR("OnException of %s\n restart server", typeid(*pThread).name());
	pThread->join();
	pThread->start();
}
