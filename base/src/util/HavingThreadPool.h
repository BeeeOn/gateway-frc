#ifndef BEEEON_HAVING_THREAD_POOL_H
#define BEEEON_HAVING_THREAD_POOL_H

#include <Poco/SharedPtr.h>
#include <Poco/ThreadPool.h>

#include "util/Loggable.h"

namespace BeeeOn {

class HavingThreadPool : protected virtual Loggable {
public:
	HavingThreadPool();
	virtual ~HavingThreadPool();

	void setMinThreads(int min);
	void setMaxThreads(int max);
	void setThreadIdleTime(int ms);

	Poco::ThreadPool &pool();

protected:
	void initPool();

private:
	int m_minThreads;
	int m_maxThreads;
	int m_threadIdleTime;
	Poco::SharedPtr<Poco::ThreadPool> m_pool;
};

}

#endif
