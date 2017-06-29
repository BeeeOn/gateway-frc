#include <ostream>

#include "io/Printable.h"

using namespace std;
using namespace BeeeOn;

IOSPrintable::IOSPrintable(ostream &out):
	m_out(out)
{
}

void IOSPrintable::print(const string &text, bool newline)
{
	m_out << text;
	if (newline)
		m_out << endl;
}
