#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/Hash.h>

#include "util/HashedLock.h"

using namespace std;

namespace BeeeOn {

class TestingLock {
public:
	TestingLock():
		m_value(0)
	{
	}

	void lock()
	{
		m_value += 1;
	}

	void unlock()
	{
		m_value -= 1;
	}

	int value() const
	{
		return m_value;
	}
private:
	int m_value;
};

class HashedLockTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(HashedLockTest);
	CPPUNIT_TEST(testSingleLock);
	CPPUNIT_TEST(testManyLocks);
	CPPUNIT_TEST(testDelayedInit);
	CPPUNIT_TEST_SUITE_END();
public:
	typedef HashedLock<TestingLock, Poco::UInt32, Poco::Hash<Poco::UInt32>> LockManager;

	void testSingleLock();
	void testManyLocks();
	void testDelayedInit();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HashedLockTest);

/**
 * With single lock, all IDs are mapping to the same lock.
 */
void HashedLockTest::testSingleLock()
{
	LockManager lock(1);

	CPPUNIT_ASSERT_EQUAL(0, lock.find(0).value());
	CPPUNIT_ASSERT_EQUAL(0, lock.find(1).value());
	CPPUNIT_ASSERT_EQUAL(0, lock.find(2).value());
	CPPUNIT_ASSERT_EQUAL(0, lock.find(3).value());

	// single ID

	lock.find(0).lock();
	CPPUNIT_ASSERT_EQUAL(1, lock.find(0).value());

	lock.find(0).unlock();
	CPPUNIT_ASSERT_EQUAL(0, lock.find(0).value());

	// mix different IDs

	lock.find(0).lock();
	CPPUNIT_ASSERT_EQUAL(1, lock.find(0).value());

	lock.find(1).lock();
	CPPUNIT_ASSERT_EQUAL(2, lock.find(1).value());

	lock.find(2).lock();
	CPPUNIT_ASSERT_EQUAL(3, lock.find(2).value());

	lock.find(3).lock();
	CPPUNIT_ASSERT_EQUAL(4, lock.find(3).value());

	lock.find(3).unlock();
	CPPUNIT_ASSERT_EQUAL(3, lock.find(3).value());

	lock.find(2).unlock();
	CPPUNIT_ASSERT_EQUAL(2, lock.find(2).value());

	lock.find(1).unlock();
	CPPUNIT_ASSERT_EQUAL(1, lock.find(1).value());

	lock.find(0).unlock();
	CPPUNIT_ASSERT_EQUAL(0, lock.find(0).value());
}

/**
 * Assume that the hash is perfect - thus it distributes numbers
 * ver well. In such case, if we lock 32-times more locks, then
 * each lock is locked exactly 32-times.
 */
void HashedLockTest::testManyLocks()
{
	LockManager lock(32);

	for (unsigned int i = 0; i < lock.size() * 32; ++i) {
		lock.find(i).lock();
	}

	for (unsigned int i = 0; i < lock.size(); ++i) {
		CPPUNIT_ASSERT_EQUAL(32, lock.at(i).value());
	}

	for (unsigned int i = 0; i < lock.size() * 32; ++i) {
		lock.find(i).lock();
	}
}

void HashedLockTest::testDelayedInit()
{
	LockManager lock;

	CPPUNIT_ASSERT_THROW(
		lock.find(0),
		Poco::IllegalStateException
	);

	CPPUNIT_ASSERT_THROW(
		lock.at(0),
		Poco::IllegalStateException
	);

	CPPUNIT_ASSERT_EQUAL(0U, lock.size());

	CPPUNIT_ASSERT_THROW(
		lock.delayedInit(0),
		Poco::InvalidArgumentException
	);

	CPPUNIT_ASSERT_NO_THROW(lock.delayedInit(10));

	CPPUNIT_ASSERT_THROW(
		lock.delayedInit(30),
		Poco::InvalidAccessException
	);
}

}
