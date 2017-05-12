#include <Poco/Logger.h>
#include <Poco/ScopedLock.h>

#include "di/Injectable.h"
#include "util/AsyncExecutor.h"

BEEEON_OBJECT_BEGIN(BeeeOn, AsyncExecutor)
BEEEON_OBJECT_CASTABLE(StoppableRunnable)
BEEEON_OBJECT_END(BeeeOn, AsyncExecutor)

using namespace BeeeOn;

AsyncExecutor::AsyncExecutor() :
	m_stopRequested(false)
{
}

AsyncExecutor::~AsyncExecutor()
{
	if (!m_taskQueue.empty()) {
		poco_warning(logger(),
			"deleting object with non empty queue, tasks count: "
			+ m_taskQueue.size()
			);
	}
}

void AsyncExecutor::invoke(std::function<void()> f)
{
	Poco::FastMutex::ScopedLock lock(m_queueMutex);
	m_taskQueue.push(f);
	m_wakeupEvent.set();
}

void AsyncExecutor::run()
{
	std::function<void()> task;

	while (!m_stopRequested) {
		Poco::ScopedLockWithUnlock<Poco::FastMutex> guard(m_queueMutex);

		if (m_taskQueue.empty()) {
			guard.unlock();
			m_wakeupEvent.wait();
			continue;
		}

		task = m_taskQueue.front();
		m_taskQueue.pop();

		guard.unlock();
		execute(task);

	}

	m_stopRequested = false;

	if (!m_taskQueue.empty()) {
		poco_warning(logger(), "exiting thread with non empty queue");
	}
}

void AsyncExecutor::execute(std::function<void()> task)
{
	try {
		task();
	}
	catch(Poco::Exception &ex) {
		logger().log(ex, __FILE__, __LINE__);
	}
	catch(std::exception &ex) {
		poco_critical(logger(), ex.what());
	}
	catch(...) {
		poco_critical(logger(), "unknown error");
	}
}

void AsyncExecutor::stop()
{
	m_stopRequested = true;
	m_wakeupEvent.set();
}
