#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Event.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>

#include "loop/LoopRunner.h"
#include "loop/StoppableLoop.h"
#include "loop/StoppableRunnable.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class LoopRunnerTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(LoopRunnerTest);
	CPPUNIT_TEST(testStartNothing);
	CPPUNIT_TEST(testAddLoop);
	CPPUNIT_TEST(testAddRunnable);
	CPPUNIT_TEST(testAutoStart);
	CPPUNIT_TEST_SUITE_END();
public:
	void testStartNothing();
	void testAddLoop();
	void testAddRunnable();
	void testAutoStart();
};

CPPUNIT_TEST_SUITE_REGISTRATION(LoopRunnerTest);

void LoopRunnerTest::testStartNothing()
{
	LoopRunner runner;
	runner.start();
	runner.stop();
}

class LoopWorks : public StoppableLoop {
public:
	LoopWorks():
		m_works(0),
		m_runnable(*this, &LoopWorks::itWorks)
	{
	}

	void start() override
	{
		m_thread.start(m_runnable);
	}

	void stop() override
	{
		m_thread.join();
	}

	void itWorks()
	{
		m_works = true;
		m_finished.set();
	}

public:
	Event m_finished;
	AtomicCounter m_works;

private:
	Thread m_thread;
	RunnableAdapter<LoopWorks> m_runnable;
};

class RunnableWorks : public StoppableRunnable {
public:
	RunnableWorks():
		m_works(0)
	{
	}

	void stop() override
	{
	}

	void run() override
	{
		m_works = true;
		m_finished.set();
	}

public:
	Event m_finished;
	AtomicCounter m_works;
};

void LoopRunnerTest::testAddLoop()
{
	LoopRunner runner;
	SharedPtr<LoopWorks> works(new LoopWorks);

	runner.addLoop(works);

	CPPUNIT_ASSERT(!works->m_works);

	runner.start();
	CPPUNIT_ASSERT(works->m_finished.tryWait(1000));
	runner.stop();

	CPPUNIT_ASSERT(works->m_works);
}

void LoopRunnerTest::testAddRunnable()
{
	LoopRunner runner;
	SharedPtr<RunnableWorks> works(new RunnableWorks);

	runner.addRunnable(works);

	CPPUNIT_ASSERT(!works->m_works);

	runner.start();
	CPPUNIT_ASSERT(works->m_finished.tryWait(1000));
	runner.stop();

	CPPUNIT_ASSERT(works->m_works);
}

void LoopRunnerTest::testAutoStart()
{
	LoopRunner runner;
	runner.setAutoStart(false);

	SharedPtr<LoopWorks> works(new LoopWorks);

	runner.addLoop(works);

	CPPUNIT_ASSERT(!works->m_works);

	runner.autoStart();
	CPPUNIT_ASSERT(!works->m_finished.tryWait(100));
	CPPUNIT_ASSERT(!works->m_works);

	runner.stop();
	CPPUNIT_ASSERT(!works->m_finished.tryWait(100));
	CPPUNIT_ASSERT(!works->m_works);
}

}
