#include <iostream>

#include <Poco/String.h>

#include "io/SimpleConsole.h"

using namespace std;
using namespace BeeeOn;
using namespace Poco;

SimpleConsole::SimpleConsole()
{
}

SimpleConsole::SimpleConsole(Poco::SharedPtr<Console> console):
	m_console(console)
{
}

string SimpleConsole::readLine()
{
	return session().readLine();
}

string SimpleConsole::readBytes(const unsigned int length)
{
	return session().readBytes(length);
}

void SimpleConsole::print(const string &text, bool newline)
{
	session().print(text, newline);
}

bool SimpleConsole::eof()
{
	return session().eof();
}

void SimpleConsole::close()
{
	m_session = NULL;
}

ConsoleSession &SimpleConsole::session()
{
	if (m_session.isNull())
		m_session = new ConsoleSession(*m_console);

	return *m_session;
}
