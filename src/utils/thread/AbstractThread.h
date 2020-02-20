#pragma once
#include <concrt.h>
#include "AbstractThreadListener.h"
#include <list>

class YAbstractThread{
public:
	enum class EState{
		logicError = -5,		
		exception = -4,	
		canceled = -3,	
		failed = -2, //create thread failed
		initial = -1,
		returned = 0,
		running = 1,
		paused = 2			
	};
	static constexpr int THREAD_TERMINATED = -1;
	static constexpr int THREAD_SUCCESS = 0;
	static constexpr int THREAD_LOGIC_ERROR = 1;
	static constexpr int THREAD_EXCEPTION = 2;

public:
	YAbstractThread(unsigned nThreadSN = 0);
	virtual ~YAbstractThread();

	void start(); 	
	int join();
	bool wait(DWORD milliseconds = INFINITE);
	int cancel(DWORD milliseconds = 0);
	void suspend();
	void resume();
	
	int getReturned() const;
	EState getState() const;
	HANDLE getHandle() const;
	unsigned getThreadId() const;
	unsigned getThreadNo() const;

	void addListener(std::shared_ptr<YAbstractThreadListener>& spListener);
	void removeListener(std::shared_ptr<YAbstractThreadListener>& spListener);

protected:
	HANDLE hThread;
	unsigned nThreadID;
	unsigned nThreadNo; //added to remember thread no.
	EState eState;
	int nReturned;

private:
	virtual int Run() = 0; //your async task		

	virtual void OnInitialise();
	virtual void OnStart();
	virtual void OnJoin();
	virtual void OnWait();
	virtual void OnCancel();
	virtual void OnSuspend();
	virtual void OnResume();
	virtual void OnReturn();
	virtual void OnLogicError();
	virtual void OnException();

	static unsigned __stdcall threadEntry(void* pParam);
	std::list<std::weak_ptr<YAbstractThreadListener>> listeners;
};