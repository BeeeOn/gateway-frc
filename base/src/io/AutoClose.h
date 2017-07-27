#ifndef BEEEON_AUTO_CLOSE_H
#define BEEEON_AUTO_CLOSE_H

#include <Poco/Logger.h>

#include "util/Loggable.h"

namespace BeeeOn {

template <typename Closable>
struct DefaultClose {
	void operator() (Closable &c) const
	{
		c.close();
	}
};

template <typename Closable, typename Close = DefaultClose<Closable>>
class AutoClose {
public:
	AutoClose(Closable &c):
		m_closable(c),
		m_logger(Loggable::forInstance(&c))
	{
	}

	/**
	 * Cannot copy because we would otherwise make possible
	 * to cause a double-close failure.
	 */
	AutoClose(const AutoClose &c) = delete;

	~AutoClose()
	{
		try {
			const Close close;
			close(m_closable);

			if (m_logger.debug()) {
				m_logger.debug(
					"auto-close successful",
					__FILE__, __LINE__
				);
			}
		}
		catch (...) {
			m_logger.fatal(
				"auto-close has failed",
				__FILE__, __LINE__
			);
		}
	}

	Closable *operator ->()
	{
		return &m_closable;
	}

	const Closable *operator ->() const
	{
		return &m_closable;
	}

private:
	Closable &m_closable;
	Poco::Logger &m_logger;
};

}

#endif
