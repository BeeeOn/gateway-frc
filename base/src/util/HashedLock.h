#ifndef BEEEON_HASHED_LOCK_H
#define BEEEON_HASHED_LOCK_H

#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>

#include <vector>

namespace BeeeOn {

/**
 * HashedLock is useful to reduce overhead of fine-grained locking strategy.
 * If there are too many instances that should be locked during its processing
 * and maintaining a separate lock for each instance would be expensive, this
 * class can help to reduce this cost. The instances are sorted into buckets
 * that share a single lock. The sorting is performed via a hash function.
 * Thus, if the hash function has a low collision rate, the performance
 * of this strategy would be very close to the fine-grained locking.
 *
 * Common usage with Poco::FastMutex:
 *
 *   #define GOOD_PRIME 31
 *   HashedLock<FastMutex, MyType, MyTypeHash> lock(GOOD_PRIME);
 *
 *   startThread(lock, threadWork);
 *   startThread(lock, threadWork);
 *   startThread(lock, threadWork);
 *
 *   void threadWork(auto &lock)
 *   {
 *      MyType &type = obtainOne();
 *      FastMutex::ScopedLock guard(lock.find(type));
 *      ...
 *   }
 *
 * The probability of sharing a lock depends on the number of threads (usually
 * fixed) and the quality of the used hash function.
 */
template <typename LockType, typename ID, typename Hash = typename ID::Hash>
class HashedLock {
public:
	typedef LockType Lock;
	typedef Poco::SharedPtr<HashedLock<Lock, ID, Hash>> Ptr;

	/**
	 * Create zero lock instances. It must be
	 * allocated explicitly by calling delayedInit().
	 */
	HashedLock();

	/**
	 * Create count lock instances.
	 */
	HashedLock(unsigned int count);

	/**
	 * Initializes the internal locks. This call can be called
	 * only once during its life time. Every other call would
	 * end up with Poco::InvalidAccessException.
	 */
	void delayedInit(unsigned int count);

	/**
	 * Find a lock for a given instance (or its identifier).
	 * The returned lock can be immediately used.
	 */
	Lock &find(const ID &id);

	/**
	 * Return lock at the given index.
	 */
	Lock &at(const unsigned int index);

	/**
	 * Return count of real locks.
	 */
	unsigned int size() const;

protected:
	void assureInitialized();

private:
	std::vector<Lock> m_lock;
};

template <typename Lock, typename ID, typename Hash>
HashedLock<Lock, ID, Hash>::HashedLock():
	HashedLock<Lock, ID, Hash>(0)
{
}

template <typename Lock, typename ID, typename Hash>
HashedLock<Lock, ID, Hash>::HashedLock(unsigned int count):
	m_lock(count)
{
}

template <typename Lock, typename ID, typename Hash>
void HashedLock<Lock, ID, Hash>::delayedInit(unsigned int count)
{
	if (count == 0) {
		throw Poco::InvalidArgumentException(
			"HashedLock must be initialized by non-zero count of locks");
	}

	if (m_lock.empty()) {
		m_lock = std::vector<Lock>(count);
	}
	else {
		throw Poco::InvalidAccessException(
			"delayedInit() can be called only once");
	}
}

template <typename Lock, typename ID, typename Hash>
Lock &HashedLock<Lock, ID, Hash>::find(const ID &id)
{
	assureInitialized();

	Hash hash;
	const unsigned int index = hash(id) % size();
	return at(index);
}

template <typename Lock, typename ID, typename Hash>
Lock &HashedLock<Lock, ID, Hash>::at(const unsigned int index)
{
	assureInitialized();
	return m_lock[index];
}

template <typename Lock, typename ID, typename Hash>
unsigned int HashedLock<Lock, ID, Hash>::size() const
{
	return m_lock.size();
}

template <typename Lock, typename ID, typename Hash>
void HashedLock<Lock, ID, Hash>::assureInitialized()
{
	if (m_lock.empty()) {
		throw Poco::IllegalStateException(
			"HashedLock has not been initialized yet");
	}
}

}

#endif
