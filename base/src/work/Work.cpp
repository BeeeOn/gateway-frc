#include <string>

#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include "work/Work.h"
#include "work/WorkAccess.h"

using namespace Poco;
using namespace BeeeOn;

Work::Work():
	m_state(STATE_IDLE),
	m_priority(0),
	m_sleepDuration(0),
	m_suspended(0),
	m_finished(0)
{
}

Work::Work(const ID &id):
	Entity(id),
	m_state(STATE_IDLE),
	m_priority(0),
	m_sleepDuration(0),
	m_suspended(0),
	m_finished(0)
{
}

Work::~Work()
{
}

void Work::setState(State state)
{
	setState(state, WorkWriting(this, __FILE__, __LINE__));
}

void Work::setState(State state, const WorkWriting &guard)
{
	guard.assureIs(this);
	m_state = state;
}

Work::State Work::state() const
{
	return state(WorkReading(this, __FILE__, __LINE__));
}

Work::State Work::state(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return m_state;
}

void Work::setPriority(int priority)
{
	setPriority(priority, WorkWriting(this, __FILE__, __LINE__));
}

void Work::setPriority(int priority, const WorkWriting &guard)
{
	guard.assureIs(this);
	m_priority = priority;
}

int Work::priority() const
{
	return priority(WorkReading(this, __FILE__, __LINE__));
}

int Work::priority(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return m_priority;
}

Nullable<Timestamp> Work::activationTime() const
{
	return activationTime(WorkReading(this, __FILE__, __LINE__));
}

Nullable<Timestamp> Work::activationTime(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return activationTimeUnlocked();
}

Nullable<Timestamp> Work::activationTimeUnlocked() const
{
	Nullable<Timestamp> result;

	if (!m_sleepDuration.isNull()) {
		if (m_suspended.epochMicroseconds() > 0)
			result = m_suspended + m_sleepDuration.value();
		else
			result = m_created + m_sleepDuration.value();
	}

	return result;
}

void Work::setNoSleepDuration()
{
	setNoSleepDuration(WorkWriting(this, __FILE__, __LINE__));
}

void Work::setNoSleepDuration(const WorkWriting &guard)
{
	guard.assureIs(this);
	m_sleepDuration.clear();
}

void Work::setSleepDuration(const Timespan &duration)
{
	setSleepDuration(duration,
		WorkWriting(this, __FILE__, __LINE__));
}

void Work::setSleepDuration(const Timespan &duration, const WorkWriting &guard)
{
	guard.assureIs(this);

	if (duration < 0)
		throw InvalidArgumentException("duration must not be negative");

	m_sleepDuration = duration;
}

void Work::setSleepDuration(const Nullable<Timespan> &duration)
{
	setSleepDuration(duration, WorkWriting(this, __FILE__, __LINE__));
}

void Work::setSleepDuration(const Nullable<Timespan> &duration, const WorkWriting &guard)
{
	if (duration.isNull())
		setNoSleepDuration(guard);
	else
		setSleepDuration(duration.value(), guard);
}

Nullable<Timespan> Work::sleepDuration() const
{
	return sleepDuration(WorkReading(this, __FILE__, __LINE__));
}

Nullable<Timespan> Work::sleepDuration(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return m_sleepDuration;
}

void Work::setCreated(const Timestamp &created)
{
	setCreated(created, WorkWriting(this, __FILE__, __LINE__));
}

void Work::setCreated(const Timestamp &created, const WorkWriting &guard)
{
	guard.assureIs(this);
	m_created = created;
}

Timestamp Work::created() const
{
	return created(WorkReading(this, __FILE__, __LINE__));
}

Timestamp Work::created(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return m_created;
}

void Work::setSuspended(const Timestamp &suspended)
{
	setSuspended(suspended, WorkWriting(this, __FILE__, __LINE__));
}

void Work::setSuspended(const Timestamp &suspended, const WorkWriting &guard)
{
	guard.assureIs(this);
	m_suspended = suspended;
}

Timestamp Work::suspended() const
{
	return suspended(WorkReading(this, __FILE__, __LINE__));
}

Timestamp Work::suspended(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return m_suspended;
}

void Work::setFinished(const Timestamp &finished)
{
	setFinished(finished, WorkWriting(this, __FILE__, __LINE__));
}

void Work::setFinished(const Timestamp &finished, const WorkWriting &guard)
{
	guard.assureIs(this);
	m_finished = finished;
}

Timestamp Work::finished() const
{
	return finished(WorkReading(this, __FILE__, __LINE__));
}

Timestamp Work::finished(const WorkAccess &guard) const
{
	guard.assureIs(this);
	return m_finished;
}
