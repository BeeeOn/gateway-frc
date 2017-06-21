#include <Poco/Logger.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>

#include "core/DeviceDataCollection.h"
#include "core/SimpleCollector.h"
#include "di/Injectable.h"
#include "model/DeviceID.h"
#include "util/StatisticsPrinter.h"

BEEEON_OBJECT_BEGIN(BeeeOn, SimpleCollector)
BEEEON_OBJECT_CASTABLE(AbstractCollector)
BEEEON_OBJECT_CASTABLE(DistributorListener)
BEEEON_OBJECT_CASTABLE(StoppableRunnable)
BEEEON_OBJECT_REF("printer", &SimpleCollector::setPrinter)
BEEEON_OBJECT_NUMBER("exportInterval", &SimpleCollector::setExportInterval)
BEEEON_OBJECT_END(BeeeOn, SimpleCollector)

using namespace BeeeOn;
using namespace Poco;
using namespace std;

SimpleCollector::SimpleCollector()
{
}

SimpleCollector::~SimpleCollector()
{
}

void SimpleCollector::onExport(const SensorData &data)
{
	Mutex::ScopedLock lock(m_mutexAddRecord);

	poco_debug(logger(), "collecting new data from deviceID: " + to_string(data.deviceID()));
	auto it = m_data.find(data.deviceID());
	if (it == m_data.end())
		insertData(data);
	else
		it->second.insertData(data);
}

void SimpleCollector::insertData(const SensorData &data)
{
	DeviceDataCollection dataNew(data.deviceID());
	dataNew.insertData(data);
	m_data.insert(make_pair(data.deviceID(), dataNew));
}

std::vector<StatisticsData> SimpleCollector::computeLastStats(const Poco::Timespan span)
{
	Poco::Timestamp now;

	std::vector<StatisticsData> result;
	for (auto const &device : m_data) { // for every device
		for (auto const &module : device.second) { // for every module of device
			StatisticsData stat = StatisticsData(device.first, module.moduleID());
			for (auto const &data : module) { //for every measured value
				if (Poco::Timespan(now - data.time()) <= span) {
					if (data.isValid()){
						stat.insertSample(data.value());
					}
					else {
						stat.insertInvalid();
					}
				}
				else {
					continue;
				}
			}
			result.push_back(stat);
		}
	}

	return result;
}

int SimpleCollector::devicesCount()
{
	return m_data.size();
}

int SimpleCollector::modulesCount()
{
	int result = 0;

	for (auto const &device : m_data) {
		result += device.second.size();
	}

	return result;
}
