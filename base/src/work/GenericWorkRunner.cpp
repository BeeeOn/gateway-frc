#include <Poco/Exception.h>
#include <Poco/Timestamp.h>

#include "di/Injectable.h"
#include "work/GenericWorkRunner.h"
#include "work/WorkAccess.h"
#include "work/WorkExecutor.h"
#include "work/WorkRepository.h"
#include "work/WorkScheduler.h"
#include "work/WorkSuspendThrowable.h"

BEEEON_OBJECT_BEGIN(BeeeOn, GenericWorkRunnerFactory)
BEEEON_OBJECT_CASTABLE(WorkRunnerFactory)
BEEEON_OBJECT_END(BeeeOn, GenericWorkRunnerFactory)

using namespace Poco;
using namespace BeeeOn;

GenericWorkRunner::GenericWorkRunner(WorkScheduler &scheduler):
	WorkRunner(scheduler)
{
}

GenericWorkRunner::~GenericWorkRunner()
{
}

void GenericWorkRunner::doSuspend(WorkExecuting &guard)
{
	if (logger().debug()) {
		logger().debug("work " + *m_work + " will be suspended",
				__FILE__, __LINE__);
	}

	m_work->setSuspended(Timestamp());

	// We do not set SCHEDULED because this will be done
	// by schedule anyway. If somebody cancels the work
	// before schedule() would lock the executionLock()
	// then the work would be cancelled with suspended time
	// set which is not a problem.
	guard.interrupt(__FILE__, __LINE__);
	m_scheduler.schedule(m_work);
}

void GenericWorkRunner::doFinish()
{
	if (logger().debug()) {
		logger().debug("work " + *m_work + " has finished",
				__FILE__, __LINE__);
	}

	WorkWriting guard(m_work, __FILE__, __LINE__);

	m_work->setState(Work::STATE_FINISHED, guard);
	m_work->setFinished(Timestamp(), guard);
	m_repository->store(m_work);
}

void GenericWorkRunner::doFailed()
{
	if (logger().debug()) {
		logger().debug("work " + *m_work + " has failed",
				__FILE__, __LINE__);
	}

	WorkWriting guard(m_work, __FILE__, __LINE__);

	m_work->setState(Work::STATE_FAILED, guard);
	m_work->setFinished(Timestamp(), guard);
	m_repository->store(m_work);
}

void GenericWorkRunner::run()
{
	WorkScheduler &scheduler = m_scheduler;

	try {
		execute();
	}
	catch (const Poco::Exception &e) {
		logger().log(e, __FILE__, __LINE__);
	}
	catch (const std::exception &e) {
		logger().critical(e.what(), __FILE__, __LINE__);
	}
	catch (...) {
		logger().critical("work " + *m_work + " failed to execute",
				__FILE__, __LINE__);
	}

	destroySelf();
	scheduler.notify();
}

void GenericWorkRunner::prepare()
{
	WorkWriting guard(m_work, __FILE__, __LINE__);

	m_work->setState(Work::STATE_EXECUTED, guard);
	m_repository->store(m_work);
}

void GenericWorkRunner::execute()
{
	WorkExecuting guard(m_work, __FILE__, __LINE__);

	if (logger().debug())
		logger().debug("executing work " + *m_work, __FILE__, __LINE__);

	try {
		prepare();
		m_executor->execute(m_work);
		doFinish();
	}
	catch (const WorkSuspendForEventThrowable &t) {
		m_work->setNoSleepDuration();
		doSuspend(guard);
	}
	catch (const WorkSuspendThrowable &t) {
		m_work->setSleepDuration(t.duration());
		doSuspend(guard);
	}
	catch (...) {
		doFailed();
		throw;
	}
}

WorkRunner *GenericWorkRunnerFactory::create(
		WorkScheduler &scheduler)
{
	return new GenericWorkRunner(scheduler);
}
