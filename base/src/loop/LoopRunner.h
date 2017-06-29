#ifndef BEEEON_LOOP_RUNNER_H
#define BEEEON_LOOP_RUNNER_H

#include <list>

#include <Poco/Mutex.h>

#include "loop/StoppableRunnable.h"
#include "loop/StoppableLoop.h"
#include "util/Loggable.h"

namespace BeeeOn {

class LoopRunner : public Loggable {
public:
	LoopRunner();
	~LoopRunner();

	void addRunnable(Poco::SharedPtr<StoppableRunnable> runnable);
	void addLoop(Poco::SharedPtr<StoppableLoop> loop);
	void setAutoStart(bool enable);

	void start();
	void stop();
	void autoStart();

protected:
	void stopAll(std::list<Poco::SharedPtr<StoppableLoop>> &list);

private:
	bool m_autoStart;
	Poco::FastMutex m_lock;
	std::list<Poco::SharedPtr<StoppableLoop>> m_loops;
	std::list<Poco::SharedPtr<StoppableLoop>> m_started;
};

}

#endif
