#pragma once
#include <vector>
#include <memory>
#include <utils\thread\AbstractThread.h>
#include "IOCP\IOCP.h"
#include <utils\synchronize\Event.h>
#include <mutex>
#include <log/log.h>

template<typename Tthread>
class YIOCPThreadPool : public YAbstractThreadListener, public std::enable_shared_from_this<YIOCPThreadPool<Tthread>> {
public:
	explicit YIOCPThreadPool(size_t initActiveThreads = 4, size_t maxActiveThreads = 10, size_t minActiveThreads = 4)
		:numInitActiveThreads(max(min(initActiveThreads, maxActiveThreads), minActiveThreads))
		, numMaxActiveThreads(max(maxActiveThreads, minActiveThreads))
		, numMinActiveThreads(minActiveThreads)
		, m_iocp(maxActiveThreads){
	}

	~YIOCPThreadPool() {
		stop();
	}

	void start() {
		std::lock_guard<std::mutex> lock(mtx);

		size_t nIndex = 1;
		for (size_t i = 0; i < numInitActiveThreads; ++i) {
			std::unique_ptr<Tthread> pT(new Tthread(m_iocp, nIndex++));
			activeThreads.push_back(std::move(pT));
		}

		for (size_t i = 0; i < (numMaxActiveThreads - numInitActiveThreads); ++i) {
			std::unique_ptr<Tthread> pT(new Tthread(m_iocp, nIndex++));
			idleThreads.push_back(std::move(pT));
		}

		using itType = typename std::vector<std::unique_ptr<Tthread>>::iterator;
		std::shared_ptr<YAbstractThreadListener> shared_this = std::enable_shared_from_this<YIOCPThreadPool<Tthread>>::shared_from_this();
		for (itType it = activeThreads.begin(); it != activeThreads.end(); ++it) {
			(*it)->addListener(shared_this);
			(*it)->start();
		}
	}

	void stop() {
		stopThreads();
		//cancelThreads();  //remove to avoid undefined behavior
	}

	void associateDevice(HANDLE hDevice, ULONG_PTR completionKey) {
		m_iocp.AssociateDevice(hDevice, completionKey);
	}

	void dispatch(ULONG_PTR completionKey, DWORD dwNumBytes, OVERLAPPED* pOverlapped){
		m_iocp.PostStatus(completionKey, dwNumBytes, pOverlapped);
	}

	size_t getNumOfActive() const{
		return activeThreads.size();
	}

	size_t getNumOfIdle() const{
		return idleThreads.size();
	}

	void IncreaseActiveThread() {
		std::lock_guard<std::mutex> lock(mtx);

		if (activeThreads.size() < numMaxActiveThreads && idleThreads.size() > 0) {
			activeThreads.push_back(std::move(idleThreads.back()));
			idleThreads.pop_back();
			activeThreads.back()->start();
		}
	}

	void ReduceActiveThread(size_t num) {
		std::lock_guard<std::mutex> lock(mtx);

		for(size_t i = 0; i<num && activeThreads.size()>numMinActiveThreads; ++i)
			dispatch(0, 0, NULL);

		auto it = activeThreads.begin();
		for (; it != activeThreads.end();) {
			if ((*it)->getState() != YAbstractThread::EState::running && (*it)->getState() != YAbstractThread::EState::paused
				&& activeThreads.size() > numMinActiveThreads) {
				idleThreads.push_back(std::move(*it));
				it = activeThreads.erase(it);
			}
			++it;
		}
	}

protected:
	void OnException(YAbstractThread* pThread) override {//restart thread
		LOGINFO("%s %d exception occured. restart thread", typeid(*pThread).name(), pThread->getThreadNo());
		pThread->start();
	}
	virtual void OnInitialise(YAbstractThread* pThread) override {
		LOGINFO("%s %d initialising", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnStart(YAbstractThread* pThread) override {
		LOGINFO("%s %d starting", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnJoin(YAbstractThread* pThread) override {
		LOGINFO("%s %d joining", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnWait(YAbstractThread* pThread) override {
		LOGINFO("%s %d waiting", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnCancel(YAbstractThread* pThread) override {
		LOGINFO("%s %d canceling", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnSuspend(YAbstractThread* pThread) override {
		LOGINFO("%s %d suspending", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnResume(YAbstractThread* pThread) override {
		LOGINFO("%s %d resuming", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnReturn(YAbstractThread* pThread) override {
		LOGINFO("%s %d returned", typeid(*pThread).name(), pThread->getThreadNo());
	}
	void OnLogicError(YAbstractThread* pThread) override {
		LOGINFO("%s %d returned logic error", typeid(*pThread).name(), pThread->getThreadNo());
	}

private:
	void stopThread() {
		dispatch(0, 0, NULL);
	}

	void stopThreads() {
		for (size_t i = 0; i < activeThreads.size(); ++i)
			stopThread();

		for(auto it = activeThreads.begin(); it != activeThreads.end(); ++it)
			(*it)->join();
	}

	void cancelThreads() {
		constexpr int timeToWait = 50; //wait 50ms, just approximately
		using itType = typename std::vector<std::unique_ptr<Tthread>>::iterator;
		for (itType it = activeThreads.begin(); it != activeThreads.end(); ++it) {
			(*it)->cancel(timeToWait);
		}
		//double check
		for (itType it = activeThreads.begin(); it != activeThreads.end(); ++it) {
			(*it)->cancel(timeToWait);
		}
	}

	
	YIOCP m_iocp;
	std::vector<std::unique_ptr<Tthread>> activeThreads;
	std::vector<std::unique_ptr<Tthread>> idleThreads;

	std::mutex mtx;

	const size_t numMinActiveThreads;
	const size_t numMaxActiveThreads;
	const size_t numInitActiveThreads;	
};