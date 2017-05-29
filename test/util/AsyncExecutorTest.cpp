#include <Poco/Event.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>

#include <exception>

#include <cppunit/extensions/HelperMacros.h>

#include "util/AsyncExecutor.h"

#define MAX_WAIT_TIME 10000 // 10 seconds in ms

using namespace std;
using namespace Poco;

namespace BeeeOn {

class AsyncExecutorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(AsyncExecutorTest);
	CPPUNIT_TEST(testRunEmpty);
	CPPUNIT_TEST(testRunTwice);
	CPPUNIT_TEST(testOneTask);
	CPPUNIT_TEST(testOneThenRun);
	CPPUNIT_TEST(testThousandTasks);
	CPPUNIT_TEST(testThousandTasksThenRun);
	CPPUNIT_TEST_SUITE_END();
public:
	void testRunEmpty();
	void testRunTwice();
	void testOneTask();
	void testOneThenRun();
	void testThousandTasks();
	void testThousandTasksThenRun();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AsyncExecutorTest);

void AsyncExecutorTest::testRunEmpty()
{
	Thread t;
	AsyncExecutor executor;
	t.start(executor);
	executor.stop();
	t.join();
} // to find memory leaks with Valgrind

void AsyncExecutorTest::testRunTwice()
{
	Thread t;
	AsyncExecutor executor;
	t.start(executor);
	executor.stop();
	t.join();

	t.start(executor);
	executor.stop();
	t.join();
} // to find memory leaks with Valgrind

void AsyncExecutorTest::testOneTask()
{
	Thread t;
	AsyncExecutor executor;
	Event finishEvent;
	int c = 0;
	int a = 2;
	int b = 2;

	t.start(executor);

	executor.invoke([a, b, &c, &finishEvent]() {c = a+b; finishEvent.set();});

	if (!finishEvent.tryWait(MAX_WAIT_TIME))
		CPPUNIT_FAIL("task did not completed");
	else
		CPPUNIT_ASSERT(c == (a+b));

	executor.stop();
	t.join();
}

void AsyncExecutorTest::testOneThenRun()
{
	Thread t;
	AsyncExecutor executor;
	Event finishEvent;
	int c = 0;
	int a = 2;
	int b = 2;

	executor.invoke([a, b, &c, &finishEvent]() {c = a+b; finishEvent.set();});

	t.start(executor);

	if (!finishEvent.tryWait(MAX_WAIT_TIME))
		CPPUNIT_FAIL("task did not completed");
	else
		CPPUNIT_ASSERT(c == (a+b));

	executor.stop();
	t.join();
}

void AsyncExecutorTest::testThousandTasks()
{
	Thread t;
	AsyncExecutor executor;
	Event finishEvent;
	int results[1000];

	t.start(executor);

	for(int i=0; i<1000; i++){
		executor.invoke([&results, i]() {results[i] = i;});
	}
	executor.invoke([&finishEvent]() {finishEvent.set();});

	if (!finishEvent.tryWait(MAX_WAIT_TIME)) {
		CPPUNIT_FAIL("tasks did not complete");
	}
	else {
		for(int i=0; i<1000; i++){
			CPPUNIT_ASSERT(results[i] == i);
		}
	}

	executor.stop();
	t.join();
}

void AsyncExecutorTest::testThousandTasksThenRun()
{
	Thread t;
	AsyncExecutor executor;
	Event finishEvent;
	int results[1000];

	for (int i=0; i<1000; i++){
		results[i] = 0;
		executor.invoke([&results, i]() {results[i] = i;});
	}

	for (int i=0; i<1000; i++) {
		CPPUNIT_ASSERT(results[i] == 0);
	}

	executor.invoke([&finishEvent]() {finishEvent.set();});

	t.start(executor);

	if (!finishEvent.tryWait(MAX_WAIT_TIME)) {
		CPPUNIT_FAIL("tasks did not complete");
	}
	else {
		for(int i=0; i<1000; i++){
			CPPUNIT_ASSERT(results[i] == i);
		}
	}

	executor.stop();
	t.join();
}

} // namespace BeeeOn
