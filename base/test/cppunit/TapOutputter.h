#ifndef BEEEON_TAP_OUTPUTTER_H
#define BEEEON_TAP_OUTPUTTER_H

#include <iosfwd>
#include <vector>

#include <cppunit/Outputter.h>

namespace CppUnit {

class Exception;
class Test;
class TestFailure;
class TestResultCollector;

}

namespace BeeeOn {

/**
 * Plugin to CppUnit that outputs the test results in
 * the TAP format:
 *
 *  https://testanything.org/tap-version-13-specification.html
 *
 * The output is printed after all tests are executed thus
 * measuring time of single tests is not possible by the
 * target TAP consumer. This is simplifies the implementation
 * as the CppUnit has hierachical tests structure.
 */
class TapOutputter : public CppUnit::Outputter {
public:
	TapOutputter(CppUnit::TestResultCollector *collector);
	TapOutputter(CppUnit::TestResultCollector *collector,
			std::ostream &output);

	void write() override;

protected:
	void reportSuccess(unsigned int id, CppUnit::Test *test);
	void reportFailures(unsigned int id, CppUnit::Test *test,
			const std::vector<CppUnit::TestFailure *> &fails);
	void reportException(const CppUnit::Exception *e);

private:
	CppUnit::TestResultCollector *m_collector;
	std::ostream &m_output;
};

}

#endif
