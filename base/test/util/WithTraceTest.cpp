#include <Poco/Exception.h>

#include <cppunit/extensions/HelperMacros.h>

#include "util/WithTrace.h"

using namespace std;
using namespace Poco;

using BeeeOn::Poco::WithTrace;

namespace BeeeOn {

class WithTraceTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(WithTraceTest);
	CPPUNIT_TEST(testThrowCatch);
	CPPUNIT_TEST(testTraceOf);
	CPPUNIT_TEST_SUITE_END();
public:
	void testThrowCatch();
	void testTraceOf();
};

CPPUNIT_TEST_SUITE_REGISTRATION(WithTraceTest);

void WithTraceTest::testThrowCatch()
{
	CPPUNIT_ASSERT_THROW(throw WithTrace<RuntimeException>("basic"),
			RuntimeException);
	CPPUNIT_ASSERT_THROW(throw WithTrace<IOException>("I/O"),
			IOException);
	CPPUNIT_ASSERT_THROW(throw WithTrace<RangeException>("range"),
			LogicException);
	CPPUNIT_ASSERT_THROW(throw WithTrace<Exception>("throwable"),
			Throwable);
}

void WithTraceTest::testTraceOf()
{
	if (!Backtrace::supported)
		return;

	try {
		throw Exception("test");
	} catch (const Exception &e) {
		CPPUNIT_ASSERT(Throwable::traceOf(e).size() == 0);
	}

	try {
		throw WithTrace<Exception>("test");
	} catch (const Exception &e) {
		const Backtrace &trace(Throwable::traceOf(e));

		CPPUNIT_ASSERT(trace.size() > 4);
		CPPUNIT_ASSERT(trace[0].find("Backtrace") != string::npos);
		CPPUNIT_ASSERT(trace[1].find("Throwable") != string::npos);
		CPPUNIT_ASSERT(trace[2].find("Exception") != string::npos);
		CPPUNIT_ASSERT(trace[3].find("testTraceOf") != string::npos);

		CPPUNIT_ASSERT(e.displayText().find("Exception: test: \n  ") == 0);
	}
}

}
