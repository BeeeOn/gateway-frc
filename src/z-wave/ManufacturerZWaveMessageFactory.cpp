#include <Poco/NumberParser.h>

#include "z-wave/ManufacturerZWaveMessageFactory.h"

using namespace BeeeOn;
using namespace Poco;

void ManufacturerZWaveMessageFactory::setManufacturer(
		const std::string &manufacturer)
{
	m_manufacturer = NumberParser::parseHex(manufacturer);
}

uint32_t ManufacturerZWaveMessageFactory::manufacturer() const
{
	return m_manufacturer;
}

ZWaveMessage::Ptr ManufacturerZWaveMessageFactory::create(
	const uint32_t manufacturer, const uint32_t product)
{
	if (manufacturer == m_manufacturer)
		return createByProduct(product);

	throw Poco::InvalidArgumentException(
		"invalid manufacturer: " + std::to_string(m_manufacturer));
}
