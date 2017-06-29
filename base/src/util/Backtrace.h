#ifndef BEEEON_BACKTRACE_H
#define BEEEON_BACKTRACE_H

#include <string>

#ifdef __GLIBC__
#define BEEEON_BACKTRACE_GLIBC(name, size) \
	void *name[16];                    \
	unsigned int size
#else
#define BEEEON_BACKTRACE_GLIBC(name, size)
#endif

namespace BeeeOn {

class Backtrace {
public:
	enum {
#ifdef __GLIBC__
	supported = 1
#else
	supported = 0
#endif
	};

	Backtrace(bool empty = false);
	~Backtrace();

	unsigned int size() const;

	std::string operator[] (unsigned int i) const
	{
		return at(i);
	}

	std::string at(unsigned int i) const;
	std::string toString(const std::string &indent = "") const;

protected:
	void fatal() const;

private:
BEEEON_BACKTRACE_GLIBC(m_backtrace, m_backtrace_size);
};

}

#endif
