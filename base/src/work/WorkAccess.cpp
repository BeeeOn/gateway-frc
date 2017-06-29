#include <Poco/Logger.h>

#include "work/Work.h"
#include "work/WorkAccess.h"

using namespace Poco;
using namespace BeeeOn;

WorkExecuting::WorkExecuting(const Work::Ptr work,
		const char *file, int line):
	m_work(work),
	m_guard(work->executionLock())
{
	if (logger().debug()) {
		logger().debug("begin work " + *m_work + " execution",
				file, line);
	}
}

void WorkExecuting::interrupt(const char *file, int line)
{
	if (logger().debug()) {
		logger().debug("interrupt work " + *m_work + " execution",
				file, line);
	}

	m_guard.unlock();
}

WorkExecuting::~WorkExecuting()
{
	if (logger().debug())
		logger().debug("end work " + *m_work + " execution");
}

static const std::string &wrLabel(bool write)
{
	static const std::string writing("writing");
	static const std::string reading("reading");

	return write? writing : reading;
}

static const Work *workNotNull(const Work *work)
{
	if (work == NULL)
		throw NullPointerException("work is Null while locking access");

	return work;
}

WorkAccess::WorkAccess(const Work *work, bool write):
	m_write(write),
	m_work(workNotNull(work)),
	m_guard(work->accessLock(), write)
{
	if (logger().trace())
		logger().trace(wrLabel(write) + " work " + *work);
}

WorkAccess::WorkAccess(const Work *work, bool write,
		const char *file, int line):
	m_write(write),
	m_work(workNotNull(work)),
	m_guard(work->accessLock(), write)
{
	if (logger().trace()) {
		logger().trace("begin "
			+ wrLabel(m_write) + " work " + *m_work,
			file, line);
	}
}

WorkAccess::~WorkAccess()
{
	if (logger().trace())
		logger().trace("end " + wrLabel(m_write) + " work " + *m_work);
}

void WorkAccess::assureIs(const Work *work) const
{
	if (work == NULL)
		throw NullPointerException("work is NULL");

	if (work != m_work)
		throw IllegalStateException("invalid access object for work "
				+ *work + ", expected " + *m_work);
}

WorkWriting::WorkWriting(const Work *work, const char *file, int line):
	WorkAccess(work, true, file, line),
	m_executionGuard(workNotNull(work)->executionLock())
{
	if (logger().debug()) {
		logger().debug("work " + *m_work
				+ " execution disabled",
				file, line);
	}
}

WorkWriting::WorkWriting(const Work::Ptr work, const char *file, int line):
	WorkAccess(work, true, file, line),
	m_executionGuard(workNotNull(work)->executionLock())
{
}

WorkWriting::~WorkWriting()
{
	if (logger().debug())
		logger().debug("work " + *m_work + " execution enabled");
}
