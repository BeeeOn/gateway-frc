#ifndef BEEEON_PRINTABLE_H
#define BEEEON_PRINTABLE_H

#include <iosfwd>
#include <string>

namespace BeeeOn {

class Printable {
public:
	virtual void print(const std::string &text, bool newline = true) = 0;
};

/**
 * Printable wrapper around std::ostream.
 */
class IOSPrintable : public Printable {
public:
	IOSPrintable(std::ostream &out);

	void print(const std::string &text, bool newline = true) override;

private:
	std::ostream &m_out;
};

}

#endif
