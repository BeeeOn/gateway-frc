#ifndef BEEEON_WORK_REPOSITORY_H
#define BEEEON_WORK_REPOSITORY_H

#include <vector>

#include "work/Work.h"

namespace BeeeOn {

class WorkRepository {
public:
	virtual ~WorkRepository();

	/**
	 * Store work persistently. It solves transactions automatically
	 * unless transaction is true.
	 */
	virtual void store(Work::Ptr work, bool transaction = false) = 0;

	/**
	 * Load all work that is to be executed.
	 */
	virtual void loadScheduled(std::vector<Work::Ptr> &all) = 0;
};

class EmptyWorkRepository : public WorkRepository {
public:
	void store(Work::Ptr work, bool transaction = false) override;
	void loadScheduled(std::vector<Work::Ptr> &all) override;

	static WorkRepository &instance();
};

}

#endif
