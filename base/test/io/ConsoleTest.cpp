#include <sstream>
#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "io/Console.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ConsoleTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ConsoleTest);
	CPPUNIT_TEST(testReadLine);
	CPPUNIT_TEST(testReadMultiLine);
	CPPUNIT_TEST(testEof);
	CPPUNIT_TEST_SUITE_END();
public:
	void testReadLine();
	void testReadMultiLine();
	void testEof();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ConsoleTest);

class FakeConsoleSessionImpl : public ConsoleSessionImpl {
public:
	typedef SharedPtr<FakeConsoleSessionImpl> Ptr;

	string readUntil(const char c) override
	{
		return iosReadUntil(m_in, c);
	}

	string readBytes(const unsigned int length) override
	{
		return iosReadBytes(m_in, length);
	}

	void print(const string &text, bool newline = true) override
	{
		iosPrint(m_out, text, newline);
	}

	bool eof() override
	{
		return m_in.eof();
	}

	istringstream &in()
	{
		return m_in;
	}

	ostringstream &out()
	{
		return m_out;
	}

private:
	istringstream m_in;
	ostringstream m_out;
};

class TestConsole : public Console {
public:
	TestConsole(ConsoleSessionImpl::Ptr impl):
		m_impl(impl)
	{
	}

	ConsoleSessionImpl::Ptr openSession() override
	{
		return m_impl;
	}

private:
	ConsoleSessionImpl::Ptr m_impl;
};

void ConsoleTest::testReadLine()
{
	FakeConsoleSessionImpl::Ptr impl(new FakeConsoleSessionImpl);
	TestConsole console(impl);

	impl->in().str("first line\nsecond line\nthird line\n");

	ConsoleSession session(console);

	CPPUNIT_ASSERT_EQUAL("first line", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> ", impl->out().str());
	CPPUNIT_ASSERT_EQUAL("second line", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> > ", impl->out().str());
	CPPUNIT_ASSERT_EQUAL("third line", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> > > ", impl->out().str());
	CPPUNIT_ASSERT_EQUAL("", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> > > > ", impl->out().str());
}

void ConsoleTest::testReadMultiLine()
{
	FakeConsoleSessionImpl::Ptr impl(new FakeConsoleSessionImpl);
	TestConsole console(impl);

	impl->in().str("first line\\\nsecond line\nthird line\n");

	ConsoleSession session(console);

	CPPUNIT_ASSERT_EQUAL("first linesecond line", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> ", impl->out().str());
	CPPUNIT_ASSERT_EQUAL("third line", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> > ", impl->out().str());
	CPPUNIT_ASSERT_EQUAL("", session.readLine());
	CPPUNIT_ASSERT_EQUAL("> > > ", impl->out().str());
}

void ConsoleTest::testEof()
{
	FakeConsoleSessionImpl::Ptr impl(new FakeConsoleSessionImpl);
	TestConsole console(impl);

	impl->in().str("line\n");

	ConsoleSession session(console);

	CPPUNIT_ASSERT(!session.eof());

	CPPUNIT_ASSERT_EQUAL("line", session.readLine());
	CPPUNIT_ASSERT(!session.eof());

	CPPUNIT_ASSERT_EQUAL("", session.readLine());
	CPPUNIT_ASSERT(session.eof());
}

}
