#include "util/WithTrace.h"

using namespace std;
using namespace BeeeOn;

Throwable::Throwable()
{
}

Throwable::~Throwable()
{
}

const Backtrace &Throwable::trace() const
{
	return m_backtrace;
}

const Backtrace &Throwable::emptyBacktrace()
{
	static Backtrace empty(true);
	return empty;
}
