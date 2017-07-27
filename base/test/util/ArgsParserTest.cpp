#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/ArgsParser.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ArgsParserTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ArgsParserTest);
	CPPUNIT_TEST(testWrongSetup);
	CPPUNIT_TEST(testWhitespaceDelimitersOnly);
	CPPUNIT_TEST(testWithQuotes);
	CPPUNIT_TEST(testEscapeQuotes);
	CPPUNIT_TEST(testUnclosedQuotes);
	CPPUNIT_TEST(testInvalidEscape);
	CPPUNIT_TEST(testNonStandardSetting);
	CPPUNIT_TEST_SUITE_END();
public:
	void testWrongSetup();
	void testWhitespaceDelimitersOnly();
	void testWithQuotes();
	void testEscapeQuotes();
	void testUnclosedQuotes();
	void testInvalidEscape();
	void testNonStandardSetting();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ArgsParserTest);

void ArgsParserTest::testWrongSetup()
{
	// quote and escape must be different
	CPPUNIT_ASSERT_THROW(
		ArgsParser parser(" ", '"', '"'),
		InvalidArgumentException
	);

	// quote must not be a whitespace
	CPPUNIT_ASSERT_THROW(
		ArgsParser parser(" ", ' ', '\\'),
		InvalidArgumentException
	);

	// escape must not be a whitespace
	CPPUNIT_ASSERT_THROW(
		ArgsParser parser(" ", '"', ' '),
		InvalidArgumentException
	);
}

/**
 * Test parsing of argument delimited by whitespace: " ", "\t".
 * We avoid using quoting here intentionally.
 */
void ArgsParserTest::testWhitespaceDelimitersOnly()
{
	ArgsParser parser;

	parser.parse("arg0 arg1\targ2 arg3   arg4 \t  arg5");
	CPPUNIT_ASSERT_EQUAL(6, parser.count());
	CPPUNIT_ASSERT_EQUAL("arg0", parser[0]);
	CPPUNIT_ASSERT_EQUAL("arg1", parser[1]);
	CPPUNIT_ASSERT_EQUAL("arg2", parser[2]);
	CPPUNIT_ASSERT_EQUAL("arg3", parser[3]);
	CPPUNIT_ASSERT_EQUAL("arg4", parser[4]);
	CPPUNIT_ASSERT_EQUAL("arg5", parser[5]);
}

/**
 * Test behaviour when using quotes. No quote escaping is used.
 * The arguments must be parsed correctly with respect to quotes
 * and whitespace.
 */
void ArgsParserTest::testWithQuotes()
{
	ArgsParser parser;

	parser.parse("\"arg0\" \"arg1\targ2\" \"arg3 \"  arg4 \t  ar\"g5");
	CPPUNIT_ASSERT_EQUAL(5, parser.count());
	CPPUNIT_ASSERT_EQUAL("arg0", parser[0]);
	CPPUNIT_ASSERT_EQUAL("arg1\targ2", parser[1]);
	CPPUNIT_ASSERT_EQUAL("arg3 ", parser[2]);
	CPPUNIT_ASSERT_EQUAL("arg4", parser[3]);
	CPPUNIT_ASSERT_EQUAL("ar\"g5", parser[4]);
}

/**
 * Test that quotes can be escaped by backslash. Escaping works only
 * inside quotes.
 */
void ArgsParserTest::testEscapeQuotes()
{
	ArgsParser parser;

	parser.parse(R"("arg0\"" "arg1\"\\ arg2" "arg3 "  arg4  ar"\g5)");
	CPPUNIT_ASSERT_EQUAL(5, parser.count());
	CPPUNIT_ASSERT_EQUAL("arg0\"", parser[0]);
	CPPUNIT_ASSERT_EQUAL("arg1\"\\ arg2", parser[1]);
	CPPUNIT_ASSERT_EQUAL("arg3 ", parser[2]);
	CPPUNIT_ASSERT_EQUAL("arg4", parser[3]);
	CPPUNIT_ASSERT_EQUAL("ar\"\\g5", parser[4]);
}

void ArgsParserTest::testUnclosedQuotes()
{
	ArgsParser parser;

	CPPUNIT_ASSERT_THROW(
		parser.parse(R"(")"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		parser.parse(R"("  \"322)"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		parser.parse(R"(arg0 "arg1" " )"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		parser.parse(R"(""")"),
		SyntaxException
	);

	// closing quotation must be followed by whitespace or EOF
	CPPUNIT_ASSERT_THROW(
		parser.parse(R"("text"a)"),
		SyntaxException
	);
}

void ArgsParserTest::testInvalidEscape()
{
	ArgsParser parser;

	// only \" or \\ is a valid escape
	CPPUNIT_ASSERT_THROW(
		parser.parse(R"("\ ")"),
		SyntaxException
	);

	// second escape is invalid
	CPPUNIT_ASSERT_THROW(
		parser.parse(R"("gre\\\ e")"),
		SyntaxException
	);

	// escape is incomplete
	CPPUNIT_ASSERT_THROW(
		parser.parse(R"("\)"),
		SyntaxException
	);
}

void ArgsParserTest::testNonStandardSetting()
{
	ArgsParser parser("123", ':', '|');
	parser.parse("a1b2c3d41:text:2:|:||:");

	CPPUNIT_ASSERT_EQUAL(6, parser.count());
	CPPUNIT_ASSERT_EQUAL("a", parser[0]);
	CPPUNIT_ASSERT_EQUAL("b", parser[1]);
	CPPUNIT_ASSERT_EQUAL("c", parser[2]);
	CPPUNIT_ASSERT_EQUAL("d4", parser[3]);
	CPPUNIT_ASSERT_EQUAL("text", parser[4]);
	CPPUNIT_ASSERT_EQUAL(":|", parser[5]);

	CPPUNIT_ASSERT_THROW(
		parser.parse(":anything"),
		SyntaxException
	);
}

}
