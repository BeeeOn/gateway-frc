#ifndef BEEEON_WORK_H
#define BEEEON_WORK_H

#include <Poco/SharedPtr.h>
#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>
#include <Poco/Nullable.h>
#include <Poco/Mutex.h>
#include <Poco/RWLock.h>

#include "model/Entity.h"
#include "model/GlobalID.h"

namespace BeeeOn {

class WorkExecuting;
class WorkWriting;
class WorkAccess;

class Work : public Entity<GlobalID> {
	friend WorkExecuting;
	friend WorkWriting;
	friend WorkAccess;
public:
	typedef Poco::SharedPtr<Work> Ptr;

	enum State {
		STATE_IDLE,
		STATE_SCHEDULED,
		STATE_EXECUTED,
		STATE_FINISHED,
		STATE_FAILED,
		STATE_CANCELED,
	};

	Work();
	Work(const ID &id);
	virtual ~Work();

	void setState(State state);
	void setState(State state, const WorkWriting &guard);
	State state() const;
	State state(const WorkAccess &guard) const;

	void setPriority(int priority);
	void setPriority(int priority, const WorkWriting &guard);

	/**
	 * Work priority. The higher it is the more probable to execute earlier.
	 * The default priority is 0. Thus negative priorities potentially slows down.
	 */
	int priority() const;
	int priority(const WorkAccess &guard) const;

	/**
	 * Return when this work is to be activated.
	 * Is the work has been suspended, activate it at
	 * suspended() + sleepDuration(). Otherwise, activate
	 * it as close to created() as possible.
	 */
	Poco::Nullable<Poco::Timestamp> activationTime() const;
	Poco::Nullable<Poco::Timestamp> activationTime(const WorkAccess &guard) const;

	void setNoSleepDuration();
	void setNoSleepDuration(const WorkWriting &guard);
	void setSleepDuration(const Poco::Timespan &duration);
	void setSleepDuration(const Poco::Timespan &duration, const WorkWriting &guard);
	void setSleepDuration(const Poco::Nullable<Poco::Timespan> &duration);
	void setSleepDuration(const Poco::Nullable<Poco::Timespan> &duration, const WorkWriting &guard);
	Poco::Nullable<Poco::Timespan> sleepDuration() const;
	Poco::Nullable<Poco::Timespan> sleepDuration(const WorkAccess &guard) const;

	void setCreated(const Poco::Timestamp &created);
	void setCreated(const Poco::Timestamp &created, const WorkWriting &guard);
	Poco::Timestamp created() const;
	Poco::Timestamp created(const WorkAccess &guard) const;

	void setSuspended(const Poco::Timestamp &suspended);
	void setSuspended(const Poco::Timestamp &suspended, const WorkWriting &guard);
	Poco::Timestamp suspended() const;
	Poco::Timestamp suspended(const WorkAccess &guard) const;

	void setFinished(const Poco::Timestamp &finished);
	void setFinished(const Poco::Timestamp &finished, const WorkWriting &guard);
	Poco::Timestamp finished() const;
	Poco::Timestamp finished(const WorkAccess &guard) const;

	static bool timestampValid(const Poco::Timestamp &tstamp)
	{
		return tstamp.epochMicroseconds() != 0;
	}

protected:
	Poco::Nullable<Poco::Timestamp> activationTimeUnlocked() const;

	/**
	 * All members must be read for concurrent access with
	 * multiple readers. The writer is usually the executor.
	 */
	Poco::RWLock &accessLock() const
	{
		return m_accessLock;
	}

	/**
	 * Execution lock prevents other threads to execute
	 * the same Work instance simultaneously.
	 */
	Poco::Mutex &executionLock() const
	{
		return m_executionLock;
	}

private:
	State m_state;
	int m_priority;
	Poco::Nullable<Poco::Timespan> m_sleepDuration;
	Poco::Timestamp m_created;
	Poco::Timestamp m_suspended;
	Poco::Timestamp m_finished;
	mutable Poco::Mutex m_executionLock;
	mutable Poco::RWLock m_accessLock;
};

typedef Work::ID WorkID;

}

#endif
