#pragma once

#include <map>

#include <Poco/Mutex.h>
#include <Poco/SharedPtr.h>

#include "core/AbstractCollector.h"
#include "core/DeviceDataCollection.h"
#include "util/StatisticsPrinter.h"

namespace BeeeOn {

class SensorData;
class DeviceID;

/**
 * @brief The SimpleCollector class
 * Collects data and holds them in internal structures.
 */
class SimpleCollector : public AbstractCollector
{
public:
	SimpleCollector();
	~SimpleCollector();

	/**
	 * @brief notifyListener
	 * @param data
	 * Receiving data from Distributor
	 */
	void onExport(const SensorData &data) override;

	std::vector<StatisticsData> computeLastStats(const Poco::Timespan span) override;

protected:
	int devicesCount() override;
	int modulesCount() override;

private:
	/**
	 * @brief insertData
	 * @param data
	 * This creates new item in list
	 */
	void insertData(const SensorData &data);

	std::map<DeviceID, DeviceDataCollection> m_data;
	Poco::Mutex m_mutexAddRecord;
};

}
