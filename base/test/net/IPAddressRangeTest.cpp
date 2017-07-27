#include <arpa/inet.h>
#include <netinet/in.h>
#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/Net/IPAddress.h>

#include "cppunit/BetterAssert.h"

#include "net/IPAddressRange.h"

using namespace Poco;
using namespace Poco::Net;

namespace BeeeOn {

class IPAddressRangeTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(IPAddressRangeTest);
	CPPUNIT_TEST(testNonIPv4);
	CPPUNIT_TEST(testInvalidMask);
	CPPUNIT_TEST(testValidMask);
	CPPUNIT_TEST(testMask32);
	CPPUNIT_TEST(testMask30);
	CPPUNIT_TEST(testMask27);
	CPPUNIT_TEST(testMask24);
	CPPUNIT_TEST(testMask16);
	CPPUNIT_TEST_SUITE_END();

public:
	void testNonIPv4();
	void testInvalidMask();
	void testValidMask();
	void testMask32();
	void testMask30();
	void testMask27();
	void testMask24();
	void testMask16();
};

CPPUNIT_TEST_SUITE_REGISTRATION(IPAddressRangeTest);

/*
 * @brief Test of creating IPAddressRange. It checks if the constructor throws exception
 * when accepts IPv6. Also it checks if the constructor does not throw exception when
 * inputs are correct (IPv4).
 */
void IPAddressRangeTest::testNonIPv4()
{
	CPPUNIT_ASSERT_THROW_MESSAGE("only IPv4 is supported",
		IPAddressRange range(IPAddress("fe20::abc:45"), IPAddress("255.255.255.0")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW_MESSAGE("only IPv4 is supported",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("ffff:ffff::")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW_MESSAGE("only IPv4 is supported",
		IPAddressRange range(IPAddress("fe20::abc:45"), IPAddress("ffff:ffff::")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("only IPv4 is supported",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.255.0.0")));
}

/*
 * @brief Test of creating IPAddressRange. It checks if the constructor throws exception
 * when accepts invalid net mask.
 */
void IPAddressRangeTest::testInvalidMask()
{
	CPPUNIT_ASSERT_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.255.255.1")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.0.0.255")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("0.255.255.255")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("192.168.255.255")),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("1.1.1.1")),
		InvalidArgumentException);
}

/*
 * @brief Test of creating IPAddressRange. It checks if the constructor does not throw
 * exception when accepts valid net mask.
 */
void IPAddressRangeTest::testValidMask()
{
	CPPUNIT_ASSERT_NO_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("0.0.0.0")));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("128.0.0.0")));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.0.0.0")));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.192.0.0")));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.255.255.0")));

	CPPUNIT_ASSERT_NO_THROW_MESSAGE("invalid net mask",
		IPAddressRange range(IPAddress("127.0.0.1"), IPAddress("255.255.255.255")));
}

/*
 * @brief It tests behavior when IPAddressRange get netmask with prefix /32.
 * Methods end and begin should return iterator with same address. The iterators
 * should be equal.
 */
void IPAddressRangeTest::testMask32()
{
	IPAddressRange range(IPAddress("192.168.101.1"), IPAddress("255.255.255.255"));

	auto it = range.begin();
	CPPUNIT_ASSERT(*it == IPAddress("192.168.101.1"));

	it = range.end();
	CPPUNIT_ASSERT(*it == IPAddress("192.168.101.1"));
	CPPUNIT_ASSERT(*it == range.broadcast());

	CPPUNIT_ASSERT(range.begin() == range.end());
}

/*
 * @brief It tests if the methods begin and end of IPAddressRange class
 * work correctly. It means if they return iterator with right address.
 * Also are tested operators of IPAddressIterator class.
 */
void IPAddressRangeTest::testMask30()
{
	IPAddressRange range(IPAddress("192.168.101.1"), IPAddress("255.255.255.252"));

	auto it = range.begin();

	CPPUNIT_ASSERT(it != range.end());
	CPPUNIT_ASSERT(*it == IPAddress("192.168.101.1"));
	CPPUNIT_ASSERT(++it != range.end());
	CPPUNIT_ASSERT(*it == IPAddress("192.168.101.2"));

	it = range.end();
	CPPUNIT_ASSERT(*it == range.broadcast());
}

/*
 * @brief It tests if it iterates correctly on the given range.
 * In this case it is range /27. Also it checks if it stops
 * iterating on the right address.
 */
void IPAddressRangeTest::testMask27()
{
	IPAddressRange range(IPAddress("192.168.101.1"), IPAddress("255.255.255.224"));

	struct in_addr addr;
	const uint32_t first = 0xc0a86501; // 192.168.101.1
	const uint32_t last = 0xc0a8651e;  // 192.168.101.30
	uint32_t current = first;

	for (auto &ip : range) {
		addr.s_addr = htonl(current);
		CPPUNIT_ASSERT(ip == IPAddress(&addr, sizeof(addr)));
		current += 1;
	}

	CPPUNIT_ASSERT((current - 1) == last);
	auto it = range.end();
	CPPUNIT_ASSERT(*it == range.broadcast());
}

/*
 * @brief It tests if it iterates correctly on the given range.
 * In this case it is range /24. Also it checks if it stops
 * iterating on the right address.
 */
void IPAddressRangeTest::testMask24()
{
	IPAddressRange range(IPAddress("192.168.101.1"), IPAddress("255.255.255.0"));

	struct in_addr addr;
	const uint32_t first = 0xc0a86501; // 192.168.101.1
	const uint32_t last = 0xc0a865fe;  // 192.168.101.254
	uint32_t current = first;

	for (auto &ip : range) {
		addr.s_addr = htonl(current);
		CPPUNIT_ASSERT(ip == IPAddress(&addr, sizeof(addr)));
		current += 1;
	}

	CPPUNIT_ASSERT((current - 1) == last);
	auto it = range.end();
	CPPUNIT_ASSERT(*it == range.broadcast());
}

/*
 * @brief It tests if it iterates correctly on the given range.
 * In this case it is range /16. Also it checks if it stops
 * iterating on the right address.
 */
void IPAddressRangeTest::testMask16()
{
	IPAddressRange range(IPAddress("192.168.0.1"), IPAddress("255.255.0.0"));

	struct in_addr addr;
	const uint32_t first = 0xc0a80001; // 192.168.0.1
	const uint32_t last = 0xc0a8fffe;  // 192.168.255.254
	uint32_t current = first;

	for (auto &ip : range) {
		addr.s_addr = htonl(current);
		CPPUNIT_ASSERT(ip == IPAddress(&addr, sizeof(addr)));
		current += 1;
	}

	CPPUNIT_ASSERT((current - 1) == last);
	auto it = range.end();
	CPPUNIT_ASSERT(*it == range.broadcast());
}

}
