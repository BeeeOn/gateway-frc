#include <arpa/inet.h>
#include <netinet/in.h>

#include <Poco/Exception.h>
#include <Poco/Net/IPAddress.h>

#include "net/IPAddressRange.h"

using namespace BeeeOn;
using namespace Poco;
using namespace Poco::Net;

IPAddressRange::IPAddressRange(const IPAddress& address, const IPAddress& mask)
{
	if (address.family() != IPAddress::Family::IPv4 || mask.family() != IPAddress::Family::IPv4)
		throw InvalidArgumentException("only IPv4 is supported");

	if (!isValidMask(mask))
		throw InvalidArgumentException("invalid net mask");

	m_address = address;
	m_mask = mask;
}

IPAddressRange::~IPAddressRange()
{
}

IPAddressRange::IPAddressIterator IPAddressRange::begin() const
{
	return IPAddressRange::IPAddressIterator(m_address);
}

IPAddressRange::IPAddressIterator IPAddressRange::end() const
{
	return IPAddressRange::IPAddressIterator(m_address | ~m_mask);
}

IPAddress IPAddressRange::broadcast() const
{
	return (m_address | ~m_mask);
}

IPAddress IPAddressRange::network() const
{
	return m_address & m_mask;
}

bool IPAddressRange::isValidMask(const IPAddress& mask) const
{
	const uint32_t array[33] = {0x00000000, 0x80000000, 0xc0000000, 0xe0000000, 0xf0000000, 0xf8000000,
	                            0xfc000000, 0xfe000000, 0xff000000, 0xff800000, 0xffc00000, 0xffe00000,
	                            0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000, 0xffff8000,
	                            0xffffc000, 0xffffe000, 0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00,
	                            0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0, 0xfffffff8,
	                            0xfffffffc, 0xfffffffe, 0xffffffff};
	struct in_addr *addr = (struct in_addr *) mask.addr();
	uint32_t addrRaw =  ntohl(addr->s_addr);

	for (int i = 0; i < 33; i++) {
		if (addrRaw == array[i])
			return true;
	}

	return false;
}

IPAddressRange::IPAddressIterator::IPAddressIterator(const IPAddress& address)
{
	m_currentAddress = address;
}

IPAddressRange::IPAddressIterator::~IPAddressIterator()
{
}

IPAddressRange::IPAddressIterator& IPAddressRange::IPAddressIterator::operator++()
{
	struct in_addr *addr = (struct in_addr *) m_currentAddress.addr();
	addr->s_addr = htonl(ntohl(addr->s_addr) + 1);
	m_currentAddress = IPAddress(addr, sizeof(*addr));
	return *this;
}

IPAddressRange::IPAddressIterator& IPAddressRange::IPAddressIterator::operator++(int)
{
	++(*this);
	return *this;
}

bool IPAddressRange::IPAddressIterator::operator==(const IPAddressRange::IPAddressIterator& other) const
{
	return m_currentAddress == other.m_currentAddress;
}

bool IPAddressRange::IPAddressIterator::operator!=(const IPAddressRange::IPAddressIterator& other) const
{
	return m_currentAddress != other.m_currentAddress;
}

IPAddress& IPAddressRange::IPAddressIterator::operator*()
{
	return m_currentAddress;
}
