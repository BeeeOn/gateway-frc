#include <Poco/Logger.h>

#include "util/HavingThreadPool.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

HavingThreadPool::HavingThreadPool():
	m_minThreads(1),
	m_maxThreads(16),
	m_threadIdleTime(100)
{
}

HavingThreadPool::~HavingThreadPool()
{
}

void HavingThreadPool::setMinThreads(int min)
{
	if (min < 0)
		throw InvalidArgumentException("minThreads must be non-negative");

	m_minThreads = min;
}

void HavingThreadPool::setMaxThreads(int max)
{
	if (max < 0)
		throw InvalidArgumentException("maxThreads must be non-negative");

	m_maxThreads = max;
}

void HavingThreadPool::setThreadIdleTime(int ms)
{
	if (ms <= 0)
		throw InvalidArgumentException("threadIdleTime must be greater then zero");

	if (ms > 0 && ms < 1000) {
		logger().warning("threadIdleTime's granularity is 1000 ms, treating "
				+ to_string(ms) + " as zero",
				__FILE__, __LINE__);
	}

	m_threadIdleTime = ms / 1000;
}

void HavingThreadPool::initPool()
{
	if (m_pool.isNull()) {
		logger().notice("creating thread pool min: "
			+ to_string(m_minThreads)
			+ " max: "
			+ to_string(m_maxThreads),
			__FILE__, __LINE__);

		m_pool = new ThreadPool(
			m_minThreads,
			m_maxThreads,
			m_threadIdleTime
		);
	}
}

ThreadPool &HavingThreadPool::pool()
{
	initPool();
	return *m_pool;
}
