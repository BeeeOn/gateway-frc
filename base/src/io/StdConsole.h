#ifndef BEEEON_STD_CONSOLE_H
#define BEEEON_STD_CONSOLE_H

#include <iosfwd>

#include <Poco/SharedPtr.h>
#include <Poco/Semaphore.h>

#include "io/Console.h"

namespace BeeeOn {

class StdConsoleSessionImpl : public ConsoleSessionImpl {
public:
	typedef Poco::SharedPtr<StdConsoleSessionImpl> Ptr;

	StdConsoleSessionImpl(Poco::Semaphore &semaphore);
	~StdConsoleSessionImpl();

	std::string readUntil(const char c) override;
	std::string readBytes(const unsigned int length) override;
	void print(const std::string &text, bool newline = true) override;
	bool eof() override;

private:
	Poco::Semaphore &m_semaphore;
};

/**
 * Standard I/O console.
 */
class StdConsole : public Console {
public:
	StdConsole();

protected:
	/**
	 * Waits until the semaphore is ready and then it
	 * creates a new instance of the StdConsoleSessionImpl.
	 * The semaphore is set by the StdConsoleSessionImpl's
	 * destructor to make another session available again.
	 */
	ConsoleSessionImpl::Ptr openSession() override;

private:
	/**
	 * There can be only one active session of the I/O console.
	 * The access is controlled by the semaphore.
	 */
	Poco::Semaphore m_semaphore;
};

}

#endif
