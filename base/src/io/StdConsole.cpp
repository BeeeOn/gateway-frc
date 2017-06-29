#include <iostream>

#include "di/Injectable.h"
#include "io/StdConsole.h"

BEEEON_OBJECT_BEGIN(BeeeOn, StdConsole)
BEEEON_OBJECT_CASTABLE(Console)
BEEEON_OBJECT_TEXT("eol", &StdConsole::setEol)
BEEEON_OBJECT_TEXT("skipEol", &StdConsole::setSkipEol)
BEEEON_OBJECT_TEXT("prompt", &StdConsole::setPrompt)
BEEEON_OBJECT_END(BeeeOn, StdConsole)

using namespace std;
using namespace Poco;
using namespace BeeeOn;

StdConsoleSessionImpl::StdConsoleSessionImpl(Semaphore &semaphore):
	m_semaphore(semaphore)
{
}

StdConsoleSessionImpl::~StdConsoleSessionImpl()
{
	m_semaphore.set();
}

string StdConsoleSessionImpl::readUntil(const char c)
{
	return iosReadUntil(cin, c);
}

string StdConsoleSessionImpl::readBytes(const unsigned int length)
{
	return iosReadBytes(cin, length);
}

void StdConsoleSessionImpl::print(const string &text, bool newline)
{
	iosPrint(cout, text, newline);
}

bool StdConsoleSessionImpl::eof()
{
	return cin.eof();
}

StdConsole::StdConsole():
	m_semaphore(1)
{
}

ConsoleSessionImpl::Ptr StdConsole::openSession()
{
	m_semaphore.wait();
	return new StdConsoleSessionImpl(m_semaphore);
}
