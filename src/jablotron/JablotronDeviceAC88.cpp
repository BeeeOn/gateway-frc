#include <Poco/StringTokenizer.h>

#include "jablotron/JablotronDeviceAC88.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

static const int AC88_MODULE_ID_SENSOR_STATE = 0;
static const list<ModuleType> MODULE_TYPES = {
	ModuleType(
		ModuleType::Type::TYPE_ON_OFF,
		set<ModuleType::Attribute>())
};

JablotronDeviceAC88::JablotronDeviceAC88(
		const DeviceID &deviceID, const string &name):
	JablotronDevice(deviceID, name)
{
}

SensorData JablotronDeviceAC88::extractSensorData(const string &message)
{
	StringTokenizer tokens(message, " ");
	SensorData sensorData;

	sensorData.setDeviceID(deviceID());
	sensorData.insertValue(
		SensorValue(
			ModuleID(AC88_MODULE_ID_SENSOR_STATE),
			parseValue(tokens[2])
		)
	);

	return sensorData;
}

list<ModuleType> JablotronDeviceAC88::moduleTypes()
{
	return MODULE_TYPES;
}
