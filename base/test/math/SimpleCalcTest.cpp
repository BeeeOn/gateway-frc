#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "math/SimpleCalc.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class SimpleCalcTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SimpleCalcTest);
	CPPUNIT_TEST(testConstant);
	CPPUNIT_TEST(testTermOpTerm);
	CPPUNIT_TEST(testLeadingWhitespace);
	CPPUNIT_TEST(testEndingWhitespace);
	CPPUNIT_TEST(testLongExpression);
	CPPUNIT_TEST(testMissingTerm);
	CPPUNIT_TEST(testDoubleSign);
	CPPUNIT_TEST(testGarbage);
	CPPUNIT_TEST_SUITE_END();
public:
	void testConstant();
	void testTermOpTerm();
	void testLeadingWhitespace();
	void testEndingWhitespace();
	void testLongExpression();
	void testMissingTerm();
	void testDoubleSign();
	void testGarbage();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SimpleCalcTest);

void SimpleCalcTest::testConstant()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("0"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("0.0"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate(".0"));
	CPPUNIT_ASSERT_EQUAL(1.0, calc.evaluate("1"));
	CPPUNIT_ASSERT_EQUAL(1234567890.0, calc.evaluate("1234567890"));
	CPPUNIT_ASSERT_EQUAL(123456789.0, calc.evaluate("123456789.0"));
	CPPUNIT_ASSERT_EQUAL(12345678.9, calc.evaluate("12345678.90"));
	CPPUNIT_ASSERT_EQUAL(1234567.89, calc.evaluate("1234567.890"));
	CPPUNIT_ASSERT_EQUAL(123456.789, calc.evaluate("123456.7890"));
	CPPUNIT_ASSERT_EQUAL(12345.6789, calc.evaluate("12345.67890"));
	CPPUNIT_ASSERT_EQUAL(1234.56789, calc.evaluate("1234.567890"));
	CPPUNIT_ASSERT_EQUAL(123.456789, calc.evaluate("123.4567890"));
	CPPUNIT_ASSERT_EQUAL(12.3456789, calc.evaluate("12.34567890"));
	CPPUNIT_ASSERT_EQUAL(1.23456789, calc.evaluate("1.234567890"));
	CPPUNIT_ASSERT_EQUAL(.123456789, calc.evaluate(".1234567890"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("-0"));
	CPPUNIT_ASSERT_EQUAL(-1.0, calc.evaluate("-1"));
}

void SimpleCalcTest::testTermOpTerm()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("0 + 0"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("0 - 0"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("0 * 0"));
	CPPUNIT_ASSERT_THROW(
		calc.evaluate("0 / 0"),
		IllegalStateException
	);

	CPPUNIT_ASSERT_EQUAL(2.0, calc.evaluate("1 + 1"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("1 - 1"));
	CPPUNIT_ASSERT_EQUAL(1.0, calc.evaluate("1 * 1"));
	CPPUNIT_ASSERT_EQUAL(1.0, calc.evaluate("1 / 1"));

	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("-1 + 1"));
	CPPUNIT_ASSERT_EQUAL(-2.0, calc.evaluate("-1 - 1"));
	CPPUNIT_ASSERT_EQUAL(-1.0, calc.evaluate("-1 * 1"));
	CPPUNIT_ASSERT_EQUAL(-1.0, calc.evaluate("-1 / 1"));

	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("1 + -1"));
	CPPUNIT_ASSERT_EQUAL(2.0, calc.evaluate("1 - -1"));
	CPPUNIT_ASSERT_EQUAL(-1.0, calc.evaluate("1 * -1"));
	CPPUNIT_ASSERT_EQUAL(-1.0, calc.evaluate("1 / -1"));

	CPPUNIT_ASSERT_EQUAL(2.5, calc.evaluate("1.5 + 1"));
	CPPUNIT_ASSERT_EQUAL(0.5, calc.evaluate("1.5 - 1"));
	CPPUNIT_ASSERT_EQUAL(1.5, calc.evaluate("1.5 * 1"));
	CPPUNIT_ASSERT_EQUAL(1.5, calc.evaluate("1.5 / 1"));

	CPPUNIT_ASSERT_EQUAL(-0.5, calc.evaluate("-1.5 + 1"));
	CPPUNIT_ASSERT_EQUAL(-2.5, calc.evaluate("-1.5 - 1"));
	CPPUNIT_ASSERT_EQUAL(-1.5, calc.evaluate("-1.5 * 1"));
	CPPUNIT_ASSERT_EQUAL(-1.5, calc.evaluate("-1.5 / 1"));

	CPPUNIT_ASSERT_EQUAL(0.5, calc.evaluate("1.5 + -1"));
	CPPUNIT_ASSERT_EQUAL(2.5, calc.evaluate("1.5 - -1"));
	CPPUNIT_ASSERT_EQUAL(-1.5, calc.evaluate("1.5 * -1"));
	CPPUNIT_ASSERT_EQUAL(-1.5, calc.evaluate("1.5 / -1"));
}

void SimpleCalcTest::testLeadingWhitespace()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("     0"));
	CPPUNIT_ASSERT_EQUAL(1.0, calc.evaluate("     1"));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("     .0"));
	CPPUNIT_ASSERT_EQUAL(1243.0, calc.evaluate("     1243"));
	CPPUNIT_ASSERT_EQUAL(1243.0, calc.evaluate(" \t  1243"));
}

void SimpleCalcTest::testEndingWhitespace()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate("0   "));
	CPPUNIT_ASSERT_EQUAL(1.0, calc.evaluate("1    "));
	CPPUNIT_ASSERT_EQUAL(0.0, calc.evaluate(".0    "));
	CPPUNIT_ASSERT_EQUAL(123.4, calc.evaluate("123.4    "));
	CPPUNIT_ASSERT_EQUAL(1243.0, calc.evaluate("1243     "));
	CPPUNIT_ASSERT_EQUAL(1243.0, calc.evaluate("1243  \t  "));
}

void SimpleCalcTest::testLongExpression()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_EQUAL(10.0, calc.evaluate("2 + 5 * 1 - 3 / 2 * 5"));
	CPPUNIT_ASSERT_EQUAL(10.0, calc.evaluate(" 2  + \t5    *\t\t 1  - 3   /  2  * 5  "));
	CPPUNIT_ASSERT_EQUAL(10.0, calc.evaluate("2+5*1-3/2*5"));

	CPPUNIT_ASSERT_EQUAL(1.0, calc.evaluate("1000 / 10 / 10 / 10"));
	CPPUNIT_ASSERT_EQUAL(1000.0, calc.evaluate("10 * 10 * 10"));
}

void SimpleCalcTest::testMissingTerm()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_THROW(
		calc.evaluate(""),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("   "),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("   ."),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("5 +"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("5 + "),
		SyntaxException
	);
}

void SimpleCalcTest::testDoubleSign()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("--"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("--1"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("5 + --1"),
		SyntaxException
	);
}

void SimpleCalcTest::testGarbage()
{
	SimpleCalc calc;

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("125q"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("0.r"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("x0"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("*0"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("/0"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("+0"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("  0 0"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("123 +1g"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		calc.evaluate("123 +1 g"),
		SyntaxException
	);
}

}
