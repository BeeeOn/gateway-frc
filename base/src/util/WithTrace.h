#ifndef BEEEON_WITH_TRACE
#define BEEEON_WITH_TRACE

#include "util/Backtrace.h"

// Avoid including Poco/Exception.h here as this is up to
// the user of the WithTrace template class.
namespace Poco {

class Exception;

}

namespace BeeeOn {

class Throwable {
public:
	Throwable();
	virtual ~Throwable();

	const Backtrace &trace() const;

	/**
	 * Return Backtrace for any object. If the object is
	 * derived from Throwable, the returned Backtrace contains
	 * information at the time of its creation. Otherwise,
	 * an empty Backtrace is returned.
	 */
	template <typename E>
	static const Backtrace &traceOf(const E &e);

protected:
	static const Backtrace &emptyBacktrace();

protected:
	Backtrace m_backtrace;
};

template <typename E>
const Backtrace &Throwable::traceOf(const E &e)
{
	try {
		const Throwable &t = dynamic_cast<const Throwable &>(e);
		return t.trace();
	} catch (...) {
		return emptyBacktrace();
	}
}

namespace Poco { // BeeeOn::Poco - only for Poco::Exception and derived classes

template <typename E>
class WithTrace : public Throwable, public E {
public:
	WithTrace(const std::string &msg, int code = 0):
		E(msg, code)
	{
		extendWithTrace();
	}

	WithTrace(const std::string &msg, const std::string &arg, int code = 0):
		E(msg, arg, code)
	{
		extendWithTrace();
	}

	WithTrace(const std::string &msg, const ::Poco::Exception &nested, int code = 0):
		E(msg, nested, code)
	{
		extendWithTrace();
	}

	WithTrace(const E &e):
		E(e)
	{
		extendWithTrace();
	}

	~WithTrace() throw()
	{
	}

private:
	void extendWithTrace()
	{
		if (trace().size() > 0)
			this->extendedMessage("\n" + trace().toString("  "));
	}
};

}

}

#endif
