#include "z-wave/products/DefaultZWaveMessage.h"

using namespace BeeeOn;
using namespace std;

SensorData DefaultZWaveMessage::extractValues(
	const vector<ZWaveSensorValue> &zwaveValues)
{
	SensorData sensorData;
	extractSensorData(sensorData, zwaveValues);

	return sensorData;
}

list<ModuleType> DefaultZWaveMessage::moduleTypes(
	const vector<ZWaveSensorValue> &zwaveValues)
{
	list<ModuleType> moduleTypes;
	extractModuleTypes(moduleTypes, zwaveValues);

	return moduleTypes;
}
