#pragma once

#include <map>

#include <Poco/SharedPtr.h>

#include "z-wave/ZWaveMessageFactory.h"

namespace BeeeOn {

class GenericZWaveMessageFactory:
	public ZWaveMessageFactory,
	public Loggable {
public:
	ZWaveMessage::Ptr create(const uint32_t manufacturer,
		const uint32_t product) override;

	/**
	 * Register Z-Wave manufacturer to factory using manufacturer id.
	 * @param factory Manufacturer object
	 */
	void registerManufacturer(
		Poco::SharedPtr<ZWaveMessageFactory> factory);

	void registerDefaultManufacturer(
		Poco::SharedPtr<ZWaveMessageFactory> factory);

private:
	std::map<uint32_t, Poco::SharedPtr<ZWaveMessageFactory>> m_manufacturers;
	Poco::SharedPtr<ZWaveMessageFactory> m_defaultManufacturer;
};

}
