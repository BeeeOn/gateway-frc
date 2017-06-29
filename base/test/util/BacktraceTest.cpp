#include <cppunit/extensions/HelperMacros.h>

#include "util/Backtrace.h"

using namespace std;

namespace BeeeOn {

class BacktraceTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(BacktraceTest);
	CPPUNIT_TEST(testBacktrace);
	CPPUNIT_TEST(testHasNames);
	CPPUNIT_TEST(testEmpty);
	CPPUNIT_TEST_SUITE_END();
public:
	void testBacktrace();
	void testHasNames();
	void testEmpty();
};

CPPUNIT_TEST_SUITE_REGISTRATION(BacktraceTest);

void BacktraceTest::testBacktrace()
{
	Backtrace trace;

	if (!Backtrace::supported) {
		CPPUNIT_ASSERT(trace.size() == 0);
		CPPUNIT_ASSERT(trace[0].empty());
		return;
	}

	CPPUNIT_ASSERT(trace.size() > 2); // somebody has to call us

	for (unsigned int i = 0; i < trace.size(); ++i)
		CPPUNIT_ASSERT(!trace[i].empty());
}

void testing_function()
{
	Backtrace trace;

	CPPUNIT_ASSERT(trace.size() > 3);

	CPPUNIT_ASSERT(trace[0].find("Backtrace") != string::npos);
	CPPUNIT_ASSERT(trace[1].find("testing_function") != string::npos);

#ifndef __arm__
	CPPUNIT_ASSERT(trace[2].find("testHasNames") != string::npos);
#endif
}

void BacktraceTest::testHasNames()
{
	if (!Backtrace::supported)
		return;

	testing_function();
}

void BacktraceTest::testEmpty()
{
	Backtrace trace(true);

	CPPUNIT_ASSERT(trace.size() == 0);
	CPPUNIT_ASSERT(trace[0].empty());
}

}
