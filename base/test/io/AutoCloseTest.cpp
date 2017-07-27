#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "io/AutoClose.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class AutoCloseTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(AutoCloseTest);
	CPPUNIT_TEST(testBlock);
	CPPUNIT_TEST(testTryCatch);
	CPPUNIT_TEST(testFailing);
	CPPUNIT_TEST_SUITE_END();
public:
	void testBlock();
	void testTryCatch();
	void testFailing();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AutoCloseTest);

class ToBeClosed {
public:
	void close()
	{
		closed = true;
	}

	bool closed = false;
};

class FailingOnClose {
public:
	void close()
	{
		throw "terrible failure";
	}
};

void AutoCloseTest::testBlock()
{
	ToBeClosed device;

	CPPUNIT_ASSERT(!device.closed);

	{
		AutoClose<ToBeClosed> wrapper(device);
		CPPUNIT_ASSERT(!device.closed);
	}

	CPPUNIT_ASSERT(device.closed);
}

void AutoCloseTest::testTryCatch()
{
	ToBeClosed device;

	CPPUNIT_ASSERT(!device.closed);

	try {
		AutoClose<ToBeClosed> wrapper(device);
		CPPUNIT_ASSERT(!device.closed);
		throw "anything";
	}
	catch (...) {
		CPPUNIT_ASSERT(device.closed);
		return;
	}

	CPPUNIT_FAIL("should never reach this point");
}

static void failClose(FailingOnClose &device)
{
	AutoClose<FailingOnClose> wrapper(device);
}

void AutoCloseTest::testFailing()
{
	FailingOnClose device;
	CPPUNIT_ASSERT_NO_THROW(failClose(device));
}

}
