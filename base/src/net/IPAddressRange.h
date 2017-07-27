#pragma once

#include <Poco/Net/IPAddress.h>

namespace BeeeOn {

/*
 * @brief The class enables iterating on Poco::Net::IPAddress class.
 * It supports only IPv4. It accepts IP address and netmask. It can
 * iterate from given IP address to last IP address identified by
 * given netmask. End method return broadcast address it means
 * this address is not processed in loop.
 *
 * In case of mask /31 and network address loop process just one
 * address and it is the network address.
 * Methods end and begin return same iterator for net mask /32.
 * Loop does not process any address in this case.
 */
class IPAddressRange {
public:
	/*
	 * @param address It starts iterate from this address.
	 * @param mask Defines the range.
	 */
	IPAddressRange(const Poco::Net::IPAddress& address, const Poco::Net::IPAddress& mask);
	~IPAddressRange();

protected:
	class IPAddressIterator {
	public:
		IPAddressIterator(const Poco::Net::IPAddress& address);
		~IPAddressIterator();

		IPAddressIterator& operator++();
		IPAddressIterator& operator++(int);
		bool operator==(const IPAddressIterator& address) const;
		bool operator!=(const IPAddressIterator& address) const;
		Poco::Net::IPAddress& operator*();

	private:
		Poco::Net::IPAddress m_currentAddress;
	};

public:
	IPAddressRange::IPAddressIterator begin() const;
	IPAddressRange::IPAddressIterator end() const;

	Poco::Net::IPAddress broadcast() const;
	Poco::Net::IPAddress network() const;
	bool isValidMask(const Poco::Net::IPAddress& mask) const;

private:
	Poco::Net::IPAddress m_address;
	Poco::Net::IPAddress m_mask;
};

}
