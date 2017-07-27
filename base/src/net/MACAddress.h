#ifndef BEEEON_MAC_ADDRESS_H
#define BEEEON_MAC_ADDRESS_H

#include <string>
#include <vector>

namespace BeeeOn {

class MACAddress {
public:
	MACAddress(const std::vector<unsigned char> &bytes);

	MACAddress(const uint64_t numMac);

	MACAddress();

	uint64_t toNumber() const;

	std::string toString() const;

	std::string toString(const char separator) const;

	std::vector<unsigned char> bytes() const
	{
		return m_bytes;
	}

	operator uint64_t() const
	{
		return toNumber();
	}

	bool operator ==(const MACAddress &mac) const
	{
		return (mac.bytes() == m_bytes);
	}

	bool operator !=(const MACAddress &mac) const
	{
		return (mac.bytes() != m_bytes);
	}

	bool operator <(const MACAddress &mac) const
	{
		return (toNumber() < mac.toNumber());
	}

	bool operator >(const MACAddress &mac) const
	{
		return (toNumber() > mac.toNumber());
	}

	static MACAddress parse(const std::string &str, const char separator = ':');

private:
	std::vector<unsigned char> m_bytes;
};

}

#endif //BEEEON_MAC_ADDRESS_H
