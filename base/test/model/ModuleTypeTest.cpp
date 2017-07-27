#include <set>
#include <cppunit/extensions/HelperMacros.h>
#include "cppunit/BetterAssert.h"

#include "model/ModuleType.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ModuleTypeTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ModuleTypeTest);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testInvalidArgumentType);
	CPPUNIT_TEST(testInvalidArgumentAttribute);
	CPPUNIT_TEST(testInvalidAttributeDuplication);
	CPPUNIT_TEST_SUITE_END();
public:
	void testParse();
	void testInvalidArgumentType();
	void testInvalidArgumentAttribute();
	void testInvalidAttributeDuplication();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ModuleTypeTest);

void ModuleTypeTest::testParse()
{
	const ModuleType &moduleType = ModuleType::parse("temperature,inner");

	CPPUNIT_ASSERT_EQUAL("temperature", moduleType.type().toString());

	for (auto &item : moduleType.attributes())
		CPPUNIT_ASSERT_EQUAL("inner", item.toString());

	CPPUNIT_ASSERT_EQUAL(1, moduleType.attributes().size());
}

void ModuleTypeTest::testInvalidArgumentType()
{
	CPPUNIT_ASSERT_THROW(ModuleType::parse("undefined-type,inner"), InvalidArgumentException);
}

void ModuleTypeTest::testInvalidArgumentAttribute()
{
	CPPUNIT_ASSERT_THROW(ModuleType::parse("temperature,undefined-attribute"), InvalidArgumentException);
}

void ModuleTypeTest::testInvalidAttributeDuplication()
{
	CPPUNIT_ASSERT_THROW(ModuleType::parse("humidity,inner,inner"), InvalidArgumentException);
}

}
