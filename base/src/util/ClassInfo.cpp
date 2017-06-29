#if defined(__clang__) || defined(__GNUC__)
#include <cxxabi.h> // abi::__cxa_demangle
#include <cstdlib>
#endif

#include "util/ClassInfo.h"

using namespace std;
using namespace BeeeOn;

ClassInfo::ClassInfo(const type_index &index):
	m_index(index)
{
}

ClassInfo::ClassInfo(const type_info &info):
	m_index(info)
{
}

string ClassInfo::id() const
{
	return m_index.name();
}

#if defined(__clang__) || defined(__GLIBCXX__)
/**
 * Use abi::__cxa_demangle to demangle the class name from typeid.
 * The abi may not be available on all compilers. The clang and
 * gcc supports this (at least in c++11).
 *
 * The displayName method does more then just demangling. It
 * also drops the template parameters. This leads to clear class
 * short names useful e.g. for logging configuration. Also occasional
 * pointer signs (*) are removed. Namespace prefixes are preserved.
 *
 * Example:
 * Test<Other> -> 4TestI5OtherE -> Test
 * Other -> 5Other -> Other
 * Other * -> P5Other -> Other
 * ?? -> PN6BeeeOn18DependencyInjectorE
 *    -> BeeeOn::DependencyInjector* -> BeeeOn::DependencyInjector
 *
 * The ?? comes probably from this pointer.
 */
string ClassInfo::name() const
{
	char *demangled = abi::__cxa_demangle(m_index.name(), 0, 0, 0);
	std::string s(demangled);
	std::free(demangled);

	std::size_t t;

	t = s.find("*<");
	if (t != std::string::npos)
		return s.substr(0, t);

	t = s.find("<");
	if (t != std::string::npos) {
		std::size_t star = s.find("*");
		if (star == std::string::npos)
			return s.substr(0, t);

		return s.substr(0, star < t? star : t);
	}

	t = s.find("*");
	return t == std::string::npos? s : s.substr(0, t);
}
#else
string ClassInfo::displayName() const
{
	return name();
}
#endif

type_index ClassInfo::index() const
{
	return m_index;
}
