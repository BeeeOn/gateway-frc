#include <cmath>
#include <string>

#include <cppunit/extensions/HelperMacros.h>
#include <Poco/Dynamic/Var.h>

#include "util/JsonUtil.h"

using namespace std;
using namespace Poco::JSON;

namespace BeeeOn {

class JsonUtilTest : public CppUnit::TestFixture {
public:
	void testExtractString();
	void testExtractDouble();
	void testExtractNullDouble();
	void testExtractInt();
	void testExtractInvalidIntValue();

private:
	CPPUNIT_TEST_SUITE(JsonUtilTest);
	CPPUNIT_TEST(testExtractString);
	CPPUNIT_TEST(testExtractDouble);
	CPPUNIT_TEST(testExtractNullDouble);
	CPPUNIT_TEST(testExtractInt);
	CPPUNIT_TEST(testExtractInvalidIntValue);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(JsonUtilTest);

/*
 * Test the parse string attribute from JSON.
 */
void JsonUtilTest::testExtractString()
{
	Object::Ptr jsonObject;

	jsonObject = JsonUtil::parse(R"({"state": "data"})");
	string state = JsonUtil::extract<string>(jsonObject, "state");
	CPPUNIT_ASSERT(state == "data");
}

/*
 * Test the parse double value from JSON attribute.
 */
void JsonUtilTest::testExtractDouble()
{
	Object::Ptr jsonObject;
	double value;

	jsonObject = JsonUtil::parse(R"({"value": "11.5"})");
	value = JsonUtil::extract<double>(jsonObject, "value");
	CPPUNIT_ASSERT(value == 11.5);

	jsonObject = JsonUtil::parse(R"({"value": 11.5})");
	value = JsonUtil::extract<double>(jsonObject, "value");
	CPPUNIT_ASSERT(value == 11.5);
}

/**
 * In JSON, the values NaN and INF are not defined. Thus, they might
 * be implemented via passing null. Because, we cannot determine whether
 * the original value was either NaN or INF, define that null means
 * NaN.
 */
void JsonUtilTest::testExtractNullDouble()
{
	Object::Ptr jsonObject;
	double value;

	jsonObject = JsonUtil::parse(R"({"value": null})");
	value = JsonUtil::extract<double>(jsonObject, "value");
	CPPUNIT_ASSERT(std::isnan(value));
}

/*
 * Test the parse int value from JSON attribute.
 */
void JsonUtilTest::testExtractInt()
{
	Object::Ptr jsonObject;
	int value;

	jsonObject = JsonUtil::parse(R"({"value": "5"})");
	value = JsonUtil::extract<int>(jsonObject, "value");
	CPPUNIT_ASSERT(value == 5);

	jsonObject = JsonUtil::parse(R"({"value": 5})");
	value = JsonUtil::extract<int>(jsonObject, "value");
	CPPUNIT_ASSERT(value == 5);
}

/*
 * Test the parse int value from JSON attribute with invalid number.
 */
void JsonUtilTest::testExtractInvalidIntValue()
{
	string testJson = R"({"value": "not a number"})";

	Object::Ptr jsonObject = JsonUtil::parse(testJson);
	CPPUNIT_ASSERT_THROW(
		JsonUtil::extract<int>(jsonObject, "value"),
		Poco::SyntaxException);
}

}
