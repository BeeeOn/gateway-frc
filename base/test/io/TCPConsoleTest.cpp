#include <sstream>
#include <cppunit/extensions/HelperMacros.h>

#include <Poco/AtomicCounter.h>
#include <Poco/Environment.h>
#include <Poco/Event.h>
#include <Poco/NumberParser.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>

#include "cppunit/BetterAssert.h"
#include "io/TCPConsole.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace BeeeOn {

#define TIMEOUT_SECS  4
#define TIMEOUT_MSECS (TIMEOUT_SECS * 1000)

/**
 * Multiple tests of sockets is unreliable on some machines
 * or operating systems and thus there is only a single test
 * that seems to work.
 */
class TCPConsoleTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TCPConsoleTest);
	CPPUNIT_TEST(testReadLine);
	CPPUNIT_TEST_SUITE_END();
public:
	TCPConsoleTest();

	void testReadLine();

protected:
	void connect(StreamSocket &socket);
	void setupConsole(TCPConsole &console) const;
	void echoLoop();
	void reconnectingLoop();

private:
	Thread m_thread;
	RunnableAdapter<TCPConsoleTest> m_echoRunnable;
	Event m_listening;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TCPConsoleTest);

TCPConsoleTest::TCPConsoleTest():
	m_echoRunnable(*this, &TCPConsoleTest::echoLoop)
{
}

void TCPConsoleTest::setupConsole(TCPConsole &console) const
{
	console.setPort(
		NumberParser::parse(
			Environment::get("TEST_TCP_CONSOLE_PORT", "6000")
		)
	);

	console.setSendTimeout(TIMEOUT_MSECS);
	console.setRecvTimeout(TIMEOUT_MSECS);
}

/**
 * Echo loop running in a separate thread.
 */
void TCPConsoleTest::echoLoop()
{
	TCPConsole console;
	setupConsole(console);
	console.startListen();

	// notify tests that we have started to listen
	m_listening.set();

	ConsoleSession session(console);

	while (!session.eof()) {
		const string &input = session.readLine();
		session.print(input);
	}
}

void TCPConsoleTest::connect(StreamSocket &socket)
{
	SocketAddress address(
		"127.0.0.1",
		NumberParser::parse(
			Environment::get("TEST_TCP_CONSOLE_PORT", "6000")
		)
	);

	socket.connect(address, Timespan(TIMEOUT_SECS * Timespan::SECONDS));
	socket.setReceiveTimeout(Timespan(TIMEOUT_SECS * Timespan::SECONDS));
}

void TCPConsoleTest::testReadLine()
{
	m_thread.start(m_echoRunnable);
	CPPUNIT_ASSERT_NO_THROW(
		m_listening.wait(TIMEOUT_MSECS)
	);

	StreamSocket socket;
	connect(socket);

	SocketInputStream in(socket);
	SocketOutputStream out(socket);

	CPPUNIT_ASSERT_EQUAL('>', in.get());
	CPPUNIT_ASSERT_EQUAL(' ', in.get());

	out << "first\n" << flush;

	CPPUNIT_ASSERT_EQUAL('f', in.get());
	CPPUNIT_ASSERT_EQUAL('i', in.get());
	CPPUNIT_ASSERT_EQUAL('r', in.get());
	CPPUNIT_ASSERT_EQUAL('s', in.get());
	CPPUNIT_ASSERT_EQUAL('t', in.get());
	CPPUNIT_ASSERT_EQUAL('\n', in.get());

	CPPUNIT_ASSERT_EQUAL('>', in.get());
	CPPUNIT_ASSERT_EQUAL(' ', in.get());

	out << "second\n" << flush;

	CPPUNIT_ASSERT_EQUAL('s', in.get());
	CPPUNIT_ASSERT_EQUAL('e', in.get());
	CPPUNIT_ASSERT_EQUAL('c', in.get());
	CPPUNIT_ASSERT_EQUAL('o', in.get());
	CPPUNIT_ASSERT_EQUAL('n', in.get());
	CPPUNIT_ASSERT_EQUAL('d', in.get());
	CPPUNIT_ASSERT_EQUAL('\n', in.get());

	CPPUNIT_ASSERT_EQUAL('>', in.get());
	CPPUNIT_ASSERT_EQUAL(' ', in.get());

	out << "third\\\nx\n" << flush;

	CPPUNIT_ASSERT_EQUAL('t', in.get());
	CPPUNIT_ASSERT_EQUAL('h', in.get());
	CPPUNIT_ASSERT_EQUAL('i', in.get());
	CPPUNIT_ASSERT_EQUAL('r', in.get());
	CPPUNIT_ASSERT_EQUAL('d', in.get());
	CPPUNIT_ASSERT_EQUAL('x', in.get());
	CPPUNIT_ASSERT_EQUAL('\n', in.get());

	CPPUNIT_ASSERT_EQUAL('>', in.get());
	CPPUNIT_ASSERT_EQUAL(' ', in.get());

	socket.close();

	CPPUNIT_ASSERT_NO_THROW(
		m_thread.join(TIMEOUT_MSECS)
	);
}

}
