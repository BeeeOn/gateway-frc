#include "core/DeviceDataCollection.h"

#include <algorithm>

using namespace BeeeOn;
using namespace std;

DeviceDataCollection::DeviceDataCollection(const DeviceID &deviceID):
	m_deviceID(deviceID)
{
}

void DeviceDataCollection::insertData(const SensorData &data)
{
	if (data.deviceID() != m_deviceID)
		return;

	for (auto const &module : data) {
		auto it = find(m_modules.begin(), m_modules.end(), module);
		if (it == m_modules.end()) {
			CollectorDataValues moduleNew(module.moduleID());
			moduleNew.insert(data.timestamp(), module.value(), module.isValid());
			m_modules.push_back(moduleNew);
		}
		else {
			it->insert(data.timestamp(), module.value(), module.isValid());
		}
	}
}

DeviceID DeviceDataCollection::deviceID() const
{
	return m_deviceID;
}

vector<CollectorDataValues>::iterator DeviceDataCollection::begin()
{
	return m_modules.begin();
}

vector<CollectorDataValues>::iterator DeviceDataCollection::end()
{
	return m_modules.end();
}

vector<CollectorDataValues>::const_iterator DeviceDataCollection::begin() const
{
	return m_modules.begin();
}

vector<CollectorDataValues>::const_iterator DeviceDataCollection::end() const
{
	return m_modules.end();
}

int DeviceDataCollection::size() const
{
	return m_modules.size();
}
