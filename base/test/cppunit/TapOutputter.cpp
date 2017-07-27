#include <iostream>
#include <map>
#include <vector>

#include <cppunit/Exception.h>
#include <cppunit/Message.h>
#include <cppunit/SourceLine.h>
#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/TestResultCollector.h>

#include "cppunit/TapOutputter.h"

using namespace std;
using namespace CppUnit;
using namespace BeeeOn;

TapOutputter::TapOutputter(TestResultCollector *collector):
	m_collector(collector),
	m_output(cout)
{
}

TapOutputter::TapOutputter(
		TestResultCollector *collector,
		ostream &output):
	m_collector(collector),
	m_output(output)
{
}

void TapOutputter::write()
{
	map<Test *, vector<TestFailure *>> failures;

	for (TestFailure *fail : m_collector->failures())
		failures[fail->failedTest()].push_back(fail);

	m_output << "1.." << m_collector->tests().size() << endl;

	unsigned int id = 1;

	for (Test *test : m_collector->tests()) {
		auto it = failures.find(test);
		if (it == failures.end())
			reportSuccess(id++, test);
		else
			reportFailures(id++, test, it->second);
	}
}

void TapOutputter::reportSuccess(unsigned int id, Test *test)
{
	m_output << "ok " << id << " - " << test->getName() << endl;
}

void TapOutputter::reportFailures(unsigned int id, Test *test,
		const vector<TestFailure *> &fails)
{
	for (const TestFailure *fail : fails) {
		const SourceLine &line = fail->sourceLine();

		m_output << " ---" << endl;

		if (!line.isValid()) {
			m_output << " file: unknown" << endl;
			m_output << " line: unknown" << endl;
		}
		else {
			m_output << " file: '" << line.fileName() << "'" << endl;
			m_output << " line: " << line.lineNumber() << endl;
		}

		if (fail->thrownException() != NULL)
			reportException(fail->thrownException());

		m_output << " ---" << endl;
	}
	
	m_output << "not ok " << id << " - " << test->getName() << endl;
}

void TapOutputter::reportException(const CppUnit::Exception *e)
{
	const Message &m = e->message();

	m_output << " message: '" << m.shortDescription() << "'" << endl;

	for (int i = 0; i < m.detailCount(); ++i)
		m_output << " detail: '" << m.detailAt(i) << "'" << endl;
}
