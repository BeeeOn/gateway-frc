#include <Poco/Exception.h>
#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/RegularExpression.h>

#include "net/MACAddress.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;
using Poco::NumberFormatter;
using Poco::RegularExpression;

MACAddress::MACAddress(const vector<unsigned char> &bytes) :
	m_bytes(bytes)
{
}

MACAddress::MACAddress(const uint64_t numMac) :
	m_bytes(6, 0)
{
	m_bytes[5] = numMac >> 40;
	m_bytes[4] = numMac >> 32;
	m_bytes[3] = numMac >> 24;
	m_bytes[2] = numMac >> 16;
	m_bytes[1] = numMac >> 8;
	m_bytes[0] = numMac;
}

MACAddress::MACAddress() :
	m_bytes(6, 0)
{
}

uint64_t MACAddress::toNumber() const
{
	uint64_t num = 0;

	for (unsigned int i = 0; i < m_bytes.size(); ++i) {
		const uint64_t byte = (unsigned char) m_bytes[i];
		num += byte << (i * 8);
	}

	return num;
}

string MACAddress::toString() const
{
	string str = "";
	for (auto b : m_bytes)
		str = NumberFormatter::formatHex(b, 2) + str;

	return str;
}

string MACAddress::toString(const char separator) const
{
	string str = "";
	for (auto b : m_bytes)
		str = NumberFormatter::formatHex(b, 2) + separator + str;

	return str.erase(str.size() - 1);
}

MACAddress MACAddress::parse(const string &str, const char separator)
{
	const string sep(1, separator);

	const RegularExpression expr(
		"([0-9A-Fa-f]{1,2})" + sep
		+ "([0-9A-Fa-f]{1,2})" + sep
		+ "([0-9A-Fa-f]{1,2})" + sep
		+ "([0-9A-Fa-f]{1,2})" + sep
		+ "([0-9A-Fa-f]{1,2})" + sep
		+ "([0-9A-Fa-f]{1,2})");

	RegularExpression::MatchVec posVec;
	if (!expr.match(str, 0, posVec))
		throw SyntaxException("invalid format of MAC address " + str);

	vector<unsigned char> bytes;

	for (int i = 6; i > 0; --i) {
		const string octet = str.substr(posVec[i].offset, posVec[i].length);
		bytes.push_back(NumberParser::parseHex(octet));
	}

	return MACAddress(bytes);
}
