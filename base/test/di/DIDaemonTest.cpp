#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "di/DIDaemon.h"

using namespace std;

namespace BeeeOn {

class TestableDIDaemon : public DIDaemon {
public:
	TestableDIDaemon():
		DIDaemon(About())
	{
	}

	using DIDaemon::handleVersion;
	using DIDaemon::versionRequested;
	using DIDaemon::handleHelp;
	using DIDaemon::helpRequested;
	using DIDaemon::handleDefine;
};

class DIDaemonTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DIDaemonTest);
	CPPUNIT_TEST(testHandleVersion);
	CPPUNIT_TEST(testHandleHelp);
	CPPUNIT_TEST(testDefineKeyValue);
	CPPUNIT_TEST(testDefineKeyWithoutValue);
	CPPUNIT_TEST(testDefineKeyValueWithEqualsSign);
	CPPUNIT_TEST_SUITE_END();
public:
	void testHandleVersion();
	void testHandleHelp();
	void testDefineKeyValue();
	void testDefineKeyWithoutValue();
	void testDefineKeyValueWithEqualsSign();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DIDaemonTest);

void DIDaemonTest::testHandleVersion()
{
	TestableDIDaemon daemon;

	CPPUNIT_ASSERT(!daemon.versionRequested());
	daemon.handleVersion("version", "");
	CPPUNIT_ASSERT(daemon.versionRequested());
}

void DIDaemonTest::testHandleHelp()
{
	TestableDIDaemon daemon;

	CPPUNIT_ASSERT(!daemon.helpRequested());
	daemon.handleHelp("help", "");
	CPPUNIT_ASSERT(daemon.helpRequested());
}


void DIDaemonTest::testDefineKeyValue()
{
	TestableDIDaemon daemon;

	CPPUNIT_ASSERT(!daemon.config().has("name"));
	daemon.handleDefine("define", "name=value");
	CPPUNIT_ASSERT_EQUAL("value", daemon.config().getString("name"));

	CPPUNIT_ASSERT(!daemon.config().has("application.name"));
	daemon.handleDefine("define", "application.name=Test");
	CPPUNIT_ASSERT_EQUAL("Test", daemon.config().getString("application.name"));
}

void DIDaemonTest::testDefineKeyWithoutValue()
{
	TestableDIDaemon daemon;

	CPPUNIT_ASSERT(!daemon.config().has("name"));
	daemon.handleDefine("define", "name=");
	CPPUNIT_ASSERT(daemon.config().has("name"));
	CPPUNIT_ASSERT(daemon.config().getString("name").empty());

	CPPUNIT_ASSERT(!daemon.config().has("name2"));
	daemon.handleDefine("define", "name2");
	CPPUNIT_ASSERT(daemon.config().has("name2"));
	CPPUNIT_ASSERT(daemon.config().getString("name2").empty());
}

void DIDaemonTest::testDefineKeyValueWithEqualsSign()
{
	TestableDIDaemon daemon;

	CPPUNIT_ASSERT(!daemon.config().has("name"));
	daemon.handleDefine("define", "name=xxx=value");
	CPPUNIT_ASSERT_EQUAL("xxx=value", daemon.config().getString("name"));
}

}
