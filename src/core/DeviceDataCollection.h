#pragma once

#include <vector>

#include "model/SensorData.h"
#include "model/ModuleID.h"
#include "core/CollectorDataValues.h"

namespace BeeeOn {

/**
 * @brief The DeviceDataCollection class
 * Summarizing data for one device
 * Contains: DeviceID and vector of module available via iterator
 * Similarly to class SensorData
 */
class DeviceDataCollection {
public:
	DeviceDataCollection(const DeviceID &deviceID);

	DeviceID deviceID() const;

	/**
	 * @brief insertData
	 * @param data
	 * Save data of modules if deviceID it's the same
	 * It's creates itself new item of CollectorDataValues
	 * when there is new module
	 */
	void insertData(const SensorData &data);

	std::vector<CollectorDataValues>::iterator begin();

	std::vector<CollectorDataValues>::iterator end();

	std::vector<CollectorDataValues>::const_iterator begin() const;

	std::vector<CollectorDataValues>::const_iterator end() const;

	int size() const;

private:
	DeviceID m_deviceID;
	std::vector<CollectorDataValues> m_modules;
};

}
