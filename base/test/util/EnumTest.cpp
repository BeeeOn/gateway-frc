#include <cppunit/extensions/HelperMacros.h>

#include "util/Enum.h"
#include "cppunit/BetterAssert.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class EnumTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(EnumTest);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testFromRaw);
	CPPUNIT_TEST(testCompare);
	CPPUNIT_TEST(testStringConcat);
	CPPUNIT_TEST_SUITE_END();
public:
	void testParse();
	void testFromRaw();
	void testCompare();
	void testStringConcat();
};

CPPUNIT_TEST_SUITE_REGISTRATION(EnumTest);

struct TestTypeEnum {
	enum Raw {
		TEST_X0 = 0,
		TEST_X1,
		TEST_X2,
		TEST_X3
	};

	static EnumHelper<Raw>::ValueMap &valueMap();
};

EnumHelper<TestTypeEnum::Raw>::ValueMap &TestTypeEnum::valueMap()
{
	static EnumHelper<TestTypeEnum::Raw>::ValueMap valueMap = {
		{TestTypeEnum::TEST_X0, "x0"},
		{TestTypeEnum::TEST_X1, "x1"},
		{TestTypeEnum::TEST_X2, "x2"},
		{TestTypeEnum::TEST_X3, "x3"},
	};

	return valueMap;
}

typedef Enum<TestTypeEnum> TestType;

void EnumTest::testParse()
{
	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X0, TestType::parse("x0"));
	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X1, TestType::parse("x1"));
	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X2, TestType::parse("x2"));
	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X3, TestType::parse("x3"));

	CPPUNIT_ASSERT_THROW(TestType::parse("x4"), InvalidArgumentException);
}

void EnumTest::testFromRaw()
{
	CPPUNIT_ASSERT_EQUAL("x0", TestType::fromRaw(TestType::TEST_X0).toString());
	CPPUNIT_ASSERT_EQUAL("x1", TestType::fromRaw(TestType::TEST_X1).toString());
	CPPUNIT_ASSERT_EQUAL("x2", TestType::fromRaw(TestType::TEST_X2).toString());
	CPPUNIT_ASSERT_EQUAL("x3", TestType::fromRaw(TestType::TEST_X3).toString());

	CPPUNIT_ASSERT_THROW(TestType::fromRaw(353), InvalidArgumentException);
}

void EnumTest::testCompare()
{
	TestType typeA(TestType::fromRaw(TestType::TEST_X0));
	TestType typeB(TestType::fromRaw(TestType::TEST_X1));
	TestType typeC(TestType::fromRaw(TestType::TEST_X0));

	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X0, typeA);
	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X1, typeB);
	CPPUNIT_ASSERT_EQUAL(TestType::TEST_X0, typeC);

	CPPUNIT_ASSERT(!(TestType::TEST_X1 == typeA));
	CPPUNIT_ASSERT(TestType::TEST_X1 != typeA);
	CPPUNIT_ASSERT(typeB != TestType::TEST_X0);

	CPPUNIT_ASSERT_EQUAL(typeA, typeA);
	CPPUNIT_ASSERT_EQUAL(typeA, typeC);

	CPPUNIT_ASSERT(typeA != typeB);
	CPPUNIT_ASSERT(typeB != typeC);

	CPPUNIT_ASSERT(typeA < typeB);
	CPPUNIT_ASSERT(typeC < typeB);
	CPPUNIT_ASSERT(typeB > typeA);
	CPPUNIT_ASSERT(typeA <= typeB);
	CPPUNIT_ASSERT(typeA <= typeA);
	CPPUNIT_ASSERT(typeB >= typeA);
	CPPUNIT_ASSERT(typeB >= typeB);
}

void EnumTest::testStringConcat()
{
	TestType a(TestType::fromRaw(TestType::TEST_X2));
	TestType b(TestType::fromRaw(TestType::TEST_X3));

	CPPUNIT_ASSERT_EQUAL("test: x2", std::string("test: ") + a);
	CPPUNIT_ASSERT_EQUAL("test: x3", std::string("test: ") + b);
	CPPUNIT_ASSERT_EQUAL("test: x2, x3", std::string("test: ") + a + ", " + b);
	CPPUNIT_ASSERT_EQUAL("test: x2", "test: " + a);
	CPPUNIT_ASSERT_EQUAL("test: x3", "test: " + b);
	CPPUNIT_ASSERT_EQUAL("test: x2, x3", "test: " + a + ", " + b);
}

}
