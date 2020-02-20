#include "pch.h"
#include "AbstractThread.h"
#include "..\..\exceptions\SystemException.h"
#include "..\..\exceptions\Exception.h"
#include <process.h>
#include <handleapi.h>
#include "..\..\log\Log.h"

#pragma warning(disable:4482)

YAbstractThread::YAbstractThread() 
	: hThread(INVALID_HANDLE_VALUE),
	nThreadID(0),
	eState(EState::initial),
	nReturned(THREAD_SUCCESS)
{
}

YAbstractThread::~YAbstractThread()
{
	if(INVALID_HANDLE_VALUE != hThread){
		if(eState == EState::running || eState == EState::paused){
			//LOG_INFO("Note: CYThread destructor: will cancel the thread");
			cancel();  //Note: won't call OnThreadCancel of subclass since subclass isn't exist anymore here
		}
		CloseHandle(hThread);
	}
}

YAbstractThread::EState YAbstractThread::getState() const
{
	return eState;
}

HANDLE YAbstractThread::getHandle() const
{
	return hThread;
}

unsigned YAbstractThread::getThreadId() const
{
	return nThreadID;
}

void YAbstractThread::addListener(std::shared_ptr<YAbstractThreadListener>& spListener)
{
	if (spListener) {
		//std::weak_ptr<YAbstractThreadListener> wp(spListener);
		listeners.emplace_back(spListener);
	}
}

void YAbstractThread::removeListener(std::shared_ptr<YAbstractThreadListener>& spListener)
{
	if (spListener) {
		auto it = std::find_if(listeners.begin(), listeners.end(), [&spListener](std::weak_ptr<YAbstractThreadListener> wp) {
			auto sp = wp.lock();
			if (!sp) return false;
			else return sp == spListener; });
		if (it != listeners.end())
			listeners.erase(it);
	}
}

int YAbstractThread::getReturned() const
{
	return nReturned;
}

void YAbstractThread::start()
{
	if(eState == EState::running || eState == EState::paused)	return;
	OnInitialise();
	nReturned = THREAD_SUCCESS;

	try{
		hThread = (HANDLE)_beginthreadex(NULL, 0, threadEntry, reinterpret_cast<void*>(this), 0, &nThreadID);
		if(INVALID_HANDLE_VALUE == hThread)
			throw YSystemException("YAbstractThread", "Start");
	}catch(YSystemException& e){
		eState = EState::failed;
		LOGFATAL(e.what());
	}
}

void YAbstractThread::suspend()
{
	if (INVALID_HANDLE_VALUE == hThread)
		return;
	if (eState != EState::running)
		return;

	try {
		if (-1 == SuspendThread(hThread))
			throw YSystemException("YAbstractThread", "suspend");

		eState = EState::paused;
		OnSuspend();
	}
	catch (YSystemException & e) {
		LOGFATAL(e.what());
	}
}

void YAbstractThread::resume()
{
	if (INVALID_HANDLE_VALUE == hThread)
		return;
	if (eState != EState::paused)
		return;

	try {
		if (-1 == ResumeThread(hThread))
			throw YSystemException("YAbstractThread", "resume");

		eState = EState::running;
		OnResume();
	}
	catch (YSystemException & e) {
		LOGFATAL(e.what());
	}
}

int YAbstractThread::join()
{
	int nRet = THREAD_SUCCESS;
	if (INVALID_HANDLE_VALUE == hThread)
		return nRet;
	if (eState != EState::running && eState != EState::paused)
		return nRet;

	OnJoin();
	WaitForSingleObject(hThread, INFINITE);
	return nReturned;
}

bool YAbstractThread::wait(DWORD milliseconds /*= INFINITE*/)
{
	if (INVALID_HANDLE_VALUE == hThread)
		return false;
	if (eState != EState::running && eState != EState::paused)
		return false;

	OnWait();
	return WAIT_OBJECT_0 == WaitForSingleObject(hThread, milliseconds);
}

int YAbstractThread::cancel(DWORD milliseconds /*= 0*/)
{
	int nRet = THREAD_SUCCESS;
	if (INVALID_HANDLE_VALUE == hThread)
		return nRet;
	if (eState != EState::running && eState != EState::paused)
		return nRet;

	if (milliseconds > 0 && WaitForSingleObject(hThread, milliseconds))
	{
		nRet = nReturned;
	}
	else {
		OnCancel();
		TerminateThread(hThread, THREAD_SUCCESS);
		eState = EState::canceled;
	}
	return nRet;
}

unsigned __stdcall YAbstractThread::threadEntry(void *pParam)
{
	YAbstractThread *pThread = reinterpret_cast<YAbstractThread *>(pParam);

	try{
		pThread->OnStart();
		pThread->eState = EState::running;
		pThread->nReturned = pThread->Run();

		switch (pThread->nReturned)
		{
		case THREAD_SUCCESS:
			pThread->OnReturn();
			pThread->eState = EState::returned;
			break;
		default:
			pThread->OnLogicError();
			pThread->eState = EState::logicError;
			throw YException("thread returned error", "YThread", "threadEntry");
			break;
		}
	}
	catch(std::exception& e){
		pThread->OnException();
		pThread->nReturned = THREAD_EXCEPTION;
		pThread->eState = EState::exception;
		LOGFATAL(_T("exception in YThread threadEntry - %s"), e.what());
	}
	catch (...) {
		pThread->OnException();
		pThread->nReturned = THREAD_EXCEPTION;
		pThread->eState = EState::exception;
		LOGFATAL(_T("exception in YThread threadEntry - unknown exception"));
	}
	return pThread->nReturned;
}

void YAbstractThread::OnInitialise()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnInitialise(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnStart()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnStart(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnSuspend()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnSuspend(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnResume()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnResume(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnJoin()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnJoin(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnWait()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnWait(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnLogicError()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnLogicError(this);
		else
			listeners.erase(it);
	}
}
void YAbstractThread::OnException()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnException(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnCancel()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnCancel(this);
		else
			listeners.erase(it);
	}
}

void YAbstractThread::OnReturn()
{
	for (std::list<std::weak_ptr<YAbstractThreadListener>>::iterator it = listeners.begin();
		it != listeners.end(); ++it) {
		std::shared_ptr<YAbstractThreadListener> spListener = it->lock();
		if (spListener)
			spListener->OnReturn(this);
		else
			listeners.erase(it);
	}
}