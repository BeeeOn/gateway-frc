#include <vector>

#include <Poco/NumberParser.h>

#include "z-wave/products/AeotecZW100ZWaveMessage.h"

static const int PIR_SENSOR_ACTUATOR = 6;
static const int PIR_SENSOR_INDEX = 4;
static const int SENSOR_PIR_INTENSITY = 2;
static const int SENSOR_INDEX_REFRESH_TIME = 111;

static const int SHAKE = 3;
static const int MOTION = 8;
static const int IDLE = 0;

static const int MODULE_SHAKE_SENSOR = 0;
static const int MODULE_PIR_SENSOR = 1;
static const int MODULE_ULTRAVIOLET = 2;
static const int MODULE_LIGHT = 3;
static const int MODULE_ROOM_TEMPERATURE = 4;
static const int MODULE_ROOM_HUMIDITY = 5;
static const int MODULE_PIR_SENSOR_SENSITIVITY = 6;
static const int MODULE_BATTERY = 7;
static const int MODULE_REFRESH_TIME = 8;

using namespace BeeeOn;
using namespace std;

AeotecZW100ZWaveMessage::AeotecZW100ZWaveMessage()
{
	setMapValue();
}

SensorData AeotecZW100ZWaveMessage::extractValues(
	const vector<ZWaveSensorValue> &zwaveValues)
{
	SensorData sensorData;
	string value;

	if (parseUltraviolet(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_ULTRAVIOLET),
			Poco::NumberParser::parseFloat(value)));

	if (parseLuminance(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_LIGHT),
			Poco::NumberParser::parseFloat(value)));

	if (parseTemperature(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_ROOM_TEMPERATURE),
			Poco::NumberParser::parseFloat(value)));

	if (parseHumidity(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_ROOM_HUMIDITY),
			Poco::NumberParser::parseFloat(value)));

	if (extractPirSensitivity(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_PIR_SENSOR_SENSITIVITY),
			Poco::NumberParser::parseFloat(value)
		));

	if (parseBatteryLevel(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_BATTERY),
		Poco::NumberParser::parseFloat(value)));

	if (extractRefreshTime(value, zwaveValues))
		sensorData.insertValue(SensorValue(
			ModuleID(MODULE_REFRESH_TIME),
			Poco::NumberParser::parseFloat(value)));

	extractDetectionSensor(sensorData, zwaveValues);

	return sensorData;
}

bool AeotecZW100ZWaveMessage::modifyValue(const ModuleID &moduleID,
	double value, const uint8_t nodeId)
{
	if (moduleID.value() == PIR_SENSOR_ACTUATOR) {
		int actuatorValue = value;

		auto search = m_pirSensor.find(to_string(actuatorValue));
		if (search == m_pirSensor.end())
			return false;

		setActuator(search->second, COMMAND_CLASS_CONFIGURATION,
			PIR_SENSOR_INDEX, nodeId);
	}
	else if (moduleID.value() == MODULE_REFRESH_TIME) {
		return setActuator(std::to_string(value), COMMAND_CLASS_CONFIGURATION,
			SENSOR_INDEX_REFRESH_TIME, nodeId);
	}

	return false;
}

bool AeotecZW100ZWaveMessage::extractPirSensitivity(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	if (!extractSpecificValue(value, COMMAND_CLASS_CONFIGURATION,
		PIR_SENSOR_INDEX, zwaveValues))
		return false;

	for (const auto &item : m_pirSensor) {
		if (item.second == value) {
			value = item.first;
			return true;
		}
	}

	return false;
}

bool AeotecZW100ZWaveMessage::extractRefreshTime(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	return extractSpecificValue(value, COMMAND_CLASS_CONFIGURATION,
	SENSOR_INDEX_REFRESH_TIME, zwaveValues);
}

void AeotecZW100ZWaveMessage::extractDetectionSensor(SensorData &sensorData,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	int sensorState = 0;

	for (const ZWaveSensorValue &item : zwaveValues) {
		if (!isEqual(item, COMMAND_CLASS_ALARM, SENSOR_INDEX_BULGAR))
			continue;

		if (!extractInt(sensorState, item))
			continue;

		if (sensorState == SHAKE) {
			sensorData.insertValue(SensorValue(ModuleID(MODULE_SHAKE_SENSOR), 1));
		} else if (sensorState == MOTION) {
			sensorData.insertValue(SensorValue(ModuleID(MODULE_PIR_SENSOR), 1));
		} else {
			sensorData.insertValue(SensorValue(ModuleID(MODULE_SHAKE_SENSOR), 0));
			sensorData.insertValue(SensorValue(ModuleID(MODULE_PIR_SENSOR), 0));
		}
	}
}

void AeotecZW100ZWaveMessage::setMapValue()
{
	m_pirSensor.insert(std::make_pair("0", "Disabled"));
	m_pirSensor.insert(std::make_pair("1", "Enabled level 1 (minimum sensitivity)"));
	m_pirSensor.insert(std::make_pair("2", "Enabled level 2"));
	m_pirSensor.insert(std::make_pair("3", "Enabled level 3"));
	m_pirSensor.insert(std::make_pair("4", "Enabled level 4"));
	m_pirSensor.insert(std::make_pair("5", "Enabled level 5 (maximum sensitivity)"));
}

void AeotecZW100ZWaveMessage::modifyValueAfterStart()
{
}

list<ModuleType> AeotecZW100ZWaveMessage::moduleTypes(
	const vector<ZWaveSensorValue> &)
{
	return list<ModuleType>();
}
