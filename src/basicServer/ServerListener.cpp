#include "pch.h"
#include "ServerListener.h"
#include <log\Log.h>
#include <utils/thread/AbstractThread.h>

void YServerListener::OnInitialise(YAbstractThread* pThread)
{
	LOGINFO("Server initialising...");
}

void YServerListener::OnStart(YAbstractThread* pThread)
{
	LOGINFO("Server starting...");
}

void YServerListener::OnJoin(YAbstractThread* pThread)
{
	LOGINFO("Server join...");
}

void YServerListener::OnWait(YAbstractThread* pThread)
{
	LOGINFO("Server waiting...");
}

void YServerListener::OnCancel(YAbstractThread* pThread)
{
	LOGINFO("Server canceling...");
}

void YServerListener::OnSuspend(YAbstractThread* pThread)
{
	LOGINFO("Server suspending...");
}

void YServerListener::OnResume(YAbstractThread* pThread)
{
	LOGINFO("Server resuming...");
}

void YServerListener::OnReturn(YAbstractThread* pThread)
{
	LOGINFO("Server returning...");
}

void YServerListener::OnLogicError(YAbstractThread* pThread)
{
	LOGINFO("Server returning with logicError...");
}

void YServerListener::OnException(YAbstractThread* pThread)
{
	LOGINFO("Server exception occured");
}
