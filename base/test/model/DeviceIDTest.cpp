#include <set>
#include <cppunit/extensions/HelperMacros.h>

#include "model/DeviceID.h"

using namespace std;

namespace BeeeOn {

class DeviceIDTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DeviceIDTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testCreate32);
	CPPUNIT_TEST(testCreateFromParts);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testParse32);
	CPPUNIT_TEST(testRandom);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();
	void testCreate();
	void testCreate32();
	void testCreateFromParts();
	void testParse();
	void testParse32();
	void testRandom();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeviceIDTest);

void DeviceIDTest::setUp()
{
}

void DeviceIDTest::tearDown()
{
}

void DeviceIDTest::testCreate()
{
	const DeviceID id(0xa201020304050607UL);

	CPPUNIT_ASSERT(!id.is32bit());
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::fromRaw(DevicePrefix::PREFIX_PRESSURE_SENSOR), id.prefix());
	CPPUNIT_ASSERT_EQUAL((uint64_t) 0x01020304050607UL, id.ident());
	CPPUNIT_ASSERT(id.toString().compare("0xa201020304050607") == 0);
}

/**
 * Create 32-bit device ID (backwards compatibility).
 */
void DeviceIDTest::testCreate32()
{
	const DeviceID id(0x00000000a2010203UL);

	CPPUNIT_ASSERT(id.is32bit());
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::fromRaw(DevicePrefix::PREFIX_PRESSURE_SENSOR), id.prefix());
	CPPUNIT_ASSERT_EQUAL((uint64_t) 0x00010203, id.ident());
	CPPUNIT_ASSERT(id.toString().compare("0xa2010203") == 0);
}

void DeviceIDTest::testCreateFromParts()
{
	const DeviceID id(
		DevicePrefix::fromRaw(DevicePrefix::PREFIX_PRESSURE_SENSOR),
		0xabcdef01234567UL);

	CPPUNIT_ASSERT(!id.is32bit());
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::fromRaw(DevicePrefix::PREFIX_PRESSURE_SENSOR), id.prefix());
	CPPUNIT_ASSERT_EQUAL((uint64_t) 0xabcdef01234567UL, id.ident());
	CPPUNIT_ASSERT(id.toString().compare("0xa2abcdef01234567") == 0);
}

void DeviceIDTest::testParse()
{
	const DeviceID &id = DeviceID::parse("0xa211223344556677");

	CPPUNIT_ASSERT(!id.is32bit());
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::fromRaw(DevicePrefix::PREFIX_PRESSURE_SENSOR), id.prefix());
	CPPUNIT_ASSERT_EQUAL((uint64_t) 0x11223344556677UL, id.ident());
}

/**
 * Parse 32-bit device ID (backwards compatibility).
 */
void DeviceIDTest::testParse32()
{
	const DeviceID &id = DeviceID::parse("0xa2112233");

	CPPUNIT_ASSERT(id.is32bit());
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::fromRaw(DevicePrefix::PREFIX_PRESSURE_SENSOR), id.prefix());
	CPPUNIT_ASSERT_EQUAL((uint64_t) 0x112233UL, id.ident());
}

void DeviceIDTest::testRandom()
{
	set<DeviceID> generated;

	for (unsigned int i = 0; i < 1000; ++i) {
		const DeviceID id(DeviceID::random(DevicePrefix::fromRaw(0xa2)));
		const set<DeviceID>::size_type count = generated.size();

		CPPUNIT_ASSERT(generated.find(id) == generated.end());

		generated.insert(id);

		CPPUNIT_ASSERT_EQUAL(count + 1, generated.size());
	}
}

}
