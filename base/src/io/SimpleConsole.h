#ifndef BEEEON_SIMPLE_CONSOLE_H
#define BEEEON_SIMPLE_CONSOLE_H

#include <iosfwd>

#include <Poco/SharedPtr.h>

#include "io/Console.h"
#include "io/Printable.h"

namespace BeeeOn {

/**
 * The SimpleConsole class just wraps another Console implementation.
 * It manages a single session inside. Note that this class is not
 * thread-safe as it is intended for very simple scenarios.
 */
class SimpleConsole : public Printable {
public:
	SimpleConsole();
	SimpleConsole(Poco::SharedPtr<Console> console);

	/**
	 * Prompt for input and read a single line.
	 * Open a session if necessary.
	 */
	std::string readLine();

	/**
	 * Read the requested number of raw bytes.
	 * Open a session if necessary.
	 */
	std::string readBytes(const unsigned int length);

	/**
	 * Print text and an optional new line to the console.
	 * Open a session if necessary.
	 */
	void print(const std::string &text, bool newline = true) override;

	/**
	 * Test whether the current session says End-of-File.
	 * Open a session if necessary.
	 */
	bool eof();

	/**
	 * Close the current session.
	 */
	void close();

	void setConsole(Poco::SharedPtr<Console> console);

protected:
	ConsoleSession &session();

private:
	Poco::SharedPtr<Console> m_console;
	Poco::SharedPtr<ConsoleSession> m_session;
};

}

#endif
