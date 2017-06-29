#include <iostream>

#include <Poco/String.h>

#include "io/Console.h"

using namespace std;
using namespace BeeeOn;
using namespace Poco;

ConsoleSessionImpl::~ConsoleSessionImpl()
{
}

bool ConsoleSessionImpl::eof()
{
	return false;
}

string ConsoleSessionImpl::iosReadUntil(istream &in, const char c)
{
	string tmp;
	getline(in, tmp, c);
	return tmp;
}

string ConsoleSessionImpl::iosReadBytes(istream &in, const unsigned int length)
{
	char *buffer = new char[length];

	try {
		in.read(buffer, length);
		string result(buffer, length);
		delete buffer;
		return result;
	} catch (...) {
		delete buffer;
		throw;
	}
}

void ConsoleSessionImpl::iosPrint(ostream &out, const string &text, bool newline)
{
	out << text;
	if (newline)
		out << endl;

	out << flush;
}

ConsoleSession::ConsoleSession(Console &console):
	m_impl(console.openSession()),
	m_eol(console.eol()),
	m_skipEol(console.skipEol()),
	m_prompt(console.prompt())
{
}

string ConsoleSession::readLine()
{
	m_impl->print(m_prompt, false);

	string line;

	while (!m_impl->eof()) {
		string tmp = m_impl->readUntil(m_eol);
		trimRightInPlace(tmp);

		if (tmp.empty())
			break;

		if (tmp.back() != m_skipEol) {
			line += tmp;
			break;
		}

		// read another line because of skipEol
		line += tmp.substr(0, tmp.size() - 1);
	}

	return line;
}

string ConsoleSession::readBytes(const unsigned int length)
{
	return m_impl->readBytes(length);
}

void ConsoleSession::print(const std::string &text, bool newline)
{
	return m_impl->print(text, newline);
}

bool ConsoleSession::eof()
{
	return m_impl->eof();
}

Console::Console():
	m_eol('\n'),
	m_skipEol('\\'),
	m_prompt("> ")
{
}

void Console::setEol(const char eol)
{
	m_eol = eol;
}

char Console::eol() const
{
	return m_eol;
}

void Console::setSkipEol(const char skipEol)
{
	m_skipEol = skipEol;
}

char Console::skipEol() const
{
	return m_skipEol;
}

void Console::setPrompt(const string &prompt)
{
	m_prompt = prompt;
}

string Console::prompt() const
{
	return m_prompt;
}
