#include <string>

#include "di/Injectable.h"
#include "z-wave/GenericZWaveMessageFactory.h"
#include "z-wave/ManufacturerZWaveMessageFactory.h"
#include "z-wave/manufacturers/DefaultZWaveMessageFactory.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

BEEEON_OBJECT_BEGIN(BeeeOn, GenericZWaveMessageFactory)
BEEEON_OBJECT_CASTABLE(ZWaveMessageFactory)
BEEEON_OBJECT_REF("registerManufacturer", &GenericZWaveMessageFactory::registerManufacturer)
BEEEON_OBJECT_REF("defaultManufacturer", &GenericZWaveMessageFactory::registerDefaultManufacturer)
BEEEON_OBJECT_END(BeeeOn, GenericZWaveMessageFactory)

ZWaveMessage::Ptr GenericZWaveMessageFactory::create(
		uint32_t manufacturer, uint32_t product)
{
	auto search = m_manufacturers.find(manufacturer);
	if (search != m_manufacturers.end())
		return search->second->create(manufacturer, product);

	logger().warning(
		"manufacturer " + to_string(manufacturer)
		+ " is not registered");

	if (m_defaultManufacturer.isNull()) {
		throw InvalidArgumentException(
			"default manufacturer is not registered");
	}

	return m_defaultManufacturer->create(manufacturer, product);
}

void GenericZWaveMessageFactory::registerManufacturer(
		SharedPtr<ZWaveMessageFactory> factory)
{
	m_manufacturers.insert(
		make_pair(
			factory.cast<ManufacturerZWaveMessageFactory>()->manufacturer(),
			factory
		)
	);
}

void GenericZWaveMessageFactory::registerDefaultManufacturer(
	SharedPtr<ZWaveMessageFactory> factory)
{
	m_defaultManufacturer = factory;
}
