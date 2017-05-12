#pragma once

#include <Poco/AtomicCounter.h>
#include <Poco/Event.h>
#include <Poco/Mutex.h>

#include <functional>
#include <queue>

#include "loop/StoppableRunnable.h"
#include "util/Loggable.h"

namespace BeeeOn {

/*
 * Class for asynchronous executing tasks.
 * */
class AsyncExecutor :
	public StoppableRunnable,
	public Loggable {
public:
	AsyncExecutor();
	~AsyncExecutor();

	/*
	 * Add task to queue for executing
	 * */
	void invoke(std::function<void()> f);

	void run() override;
	void stop() override;

private:
	void execute(std::function<void()> task);
	std::queue<std::function<void()>> m_taskQueue;
	Poco::Event m_wakeupEvent;
	Poco::FastMutex m_queueMutex;

	Poco::AtomicCounter m_stopRequested;
};

}
