#ifndef BEEEON_WORK_ACCESS_H
#define BEEEON_WORK_ACCESS_H

#include "util/Loggable.h"
#include "work/Work.h"

namespace BeeeOn {

class WorkExecuting : Loggable {
public:
	WorkExecuting(const Work::Ptr work,
			const char *file, int line);
	~WorkExecuting();

	void interrupt(const char *file, int line);

private:
	const Work::Ptr m_work;
	Poco::ScopedLockWithUnlock<Poco::Mutex> m_guard;
};

class WorkAccess : protected Loggable {
public:
	void assureIs(const Work *work) const;

protected:
	WorkAccess(const Work *work, bool write);
	WorkAccess(const Work *work, bool write,
			const char *file, int line);
	~WorkAccess();

	// disable copying of the lock
	WorkAccess(const WorkAccess &) = delete;

protected:
	bool m_write;
	const Work *m_work;
	Poco::RWLock::ScopedLock m_guard;
};

class WorkReading : public WorkAccess {
public:
	WorkReading(const Work *work, const char *file, int line):
		WorkAccess(work, false, file, line)
	{
	}

	WorkReading(const Work::Ptr work, const char *file, int line):
		WorkAccess(work, false, file, line)
	{
	}
};

class WorkWriting : public WorkAccess {
public:
	WorkWriting(const Work *work, const char *file, int line);
	WorkWriting(const Work::Ptr work, const char *file, int line);

	~WorkWriting();

private:
	Poco::Mutex::ScopedLock m_executionGuard;
};

}

#endif
