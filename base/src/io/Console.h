#ifndef BEEEON_CONSOLE_H
#define BEEEON_CONSOLE_H

#include <iosfwd>

#include <Poco/SharedPtr.h>

#include "io/Printable.h"

namespace BeeeOn {

/**
 * Implementation specific console session. The session can represent
 * a user that has logged into the system, an open socket or some other
 * context.
 */
class ConsoleSessionImpl {
public:
	typedef Poco::SharedPtr<ConsoleSessionImpl> Ptr;

	virtual ~ConsoleSessionImpl();

	/**
	 * Read and return data until the character c is reached.
	 */
	virtual std::string readUntil(const char c) = 0;

	/**
	 * Read the given amount of bytes.
	 */
	virtual std::string readBytes(const unsigned int length) = 0;

	/**
	 * Print the given string. The newline can be optionally appended.
	 */
	virtual void print(const std::string &text, bool newline = true) = 0;

	/**
	 * Return true if the session cannot read any data anymore.
	 */
	virtual bool eof();

protected:
	/**
	 * Implementation of the readUntil for std::istream.
	 */
	std::string iosReadUntil(std::istream &in, const char c);

	/**
	 * Implementation of the readBytes for std::istream.
	 */
	std::string iosReadBytes(std::istream &in, const unsigned int length);

	/**
	 * Implementation of the print for std::ostream.
	 */
	void iosPrint(std::ostream &out, const std::string &text, bool newline);
};

/**
 * Forward declaration for ConsoleSession.
 */
class Console;

/**
 * ConsoleSession represents a single session of a user. The exact
 * semantics of a session is defined by the ConsoleSessionImpl.
 * This class is a universal wrapper around the implementation
 * and provides a public API to be widely used.
 */
class ConsoleSession : public Printable {
public:
	ConsoleSession(Console &console);

	/**
	 * Print prompt and read the next line from the input.
	 * The call is always blocking.
	 */
	std::string readLine();

	/**
	 * Read up to length bytes from input.
	 * The call is always blocking.
	 */
	std::string readBytes(const unsigned int length);

	/**
	 * Print the given text to the output. The newline
	 * can be optionally appended.
	 */
	void print(const std::string &text, bool newline = true) override;

	/**
	 * Return true if there will be no input from the console.
	 */
	bool eof();

private:
	ConsoleSessionImpl::Ptr m_impl;
	char m_eol;
	char m_skipEol;
	std::string m_prompt;
};

/**
 * Console represents a common settings for a certain console
 * implementation. It creates the ConsoleSessionImpl based
 * on its settings.
 */
class Console {
	friend ConsoleSession;
public:
	Console();

	void setEol(const char c);
	char eol() const;

	void setSkipEol(const char c);
	char skipEol() const;

	void setPrompt(const std::string &prompt);
	std::string prompt() const;

protected:
	virtual ConsoleSessionImpl::Ptr openSession() = 0;

private:
	char m_eol;
	char m_skipEol;
	std::string m_prompt;
};

}

#endif
