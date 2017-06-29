#ifndef BEEEON_WORK_RUNNER_H
#define BEEEON_WORK_RUNNER_H

#include <Poco/Runnable.h>

#include "work/Work.h"

namespace BeeeOn {

class WorkExecutor;
class WorkRepository;
class WorkScheduler;

class WorkRunner : public Poco::Runnable {
public:
	WorkRunner(WorkScheduler &scheduler);
	virtual ~WorkRunner();

	void setExecutor(WorkExecutor *executor);
	void setWork(Work::Ptr work);
	void setRepository(WorkRepository *repository);

protected:
	virtual void destroySelf();

protected:
	WorkScheduler &m_scheduler;
	WorkExecutor *m_executor;
	WorkRepository *m_repository;
	Work::Ptr m_work;
};

class WorkRunnerFactory {
public:
	virtual ~WorkRunnerFactory();

	/**
	 * Create new WorkRunner for the given scheduler.
	 */
	virtual WorkRunner *create(
			WorkScheduler &scheduler) = 0;
};

class NullWorkRunnerFactory : public WorkRunnerFactory {
public:
	WorkRunner *create(WorkScheduler &scheduler) override;

	static WorkRunnerFactory &instance();
};

}

#endif
