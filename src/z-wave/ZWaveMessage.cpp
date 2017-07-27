#include <list>

#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/NumberParser.h>

#include <openzwave/Manager.h>
#include <functional>

#include "model/ModuleType.h"
#include "z-wave/ZWaveMessage.h"

#include "z-wave/ZWaveDeviceManager.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

/**
 * Z-Wave index in CommandClass 49 (COMMAND_CLASS_SENSOR_MULTILEVEL)
 */
const static int SENSOR_INDEX_BATTERY = 0;
const static int SENSOR_INDEX_HUMIDITY = 5;
const static int SENSOR_INDEX_LUMINESCENCE = 3;
const static int SENSOR_INDEX_SENSOR = 0;
const static int SENSOR_INDEX_TEMPERATURE = 1;
const static int SENSOR_INDEX_ULTRAVIOLET = 27;

const static int MAXIMUN_SENSOR_TYPE = 0x3c;

static map<int, ModuleType::Type> maps = {
	pair<int, ModuleType::Type>(1, ModuleType::Type::TYPE_TEMPERATURE),
	pair<int, ModuleType::Type>(5, ModuleType::Type::TYPE_HUMIDITY)
};

bool ZWaveMessage::modifyValue(
	const ModuleID &, double, const uint8_t)
{
	throw NotImplementedException("modifyValue not implemented");
}

void ZWaveMessage::modifyValueAfterStart()
{
	throw NotImplementedException("modifyValueAfterStart not implemented");
}

bool ZWaveMessage::asBool(const string &value)
{
	return (bool) NumberParser::parseFloat(value);
}

bool ZWaveMessage::extractFloat(double &value, const ZWaveSensorValue &item)
{
	try {
		value = NumberParser::parseFloat(item.value);;
		return true;
	}
	catch (const Exception& ex) {
		logger().error("failed to parse value " + item.value + "as a double");
		logger().log(ex, __FILE__, __LINE__);
		return false;
	}
}

bool ZWaveMessage::extractInt(int &value, const ZWaveSensorValue &item)
{
	try {
		value = NumberParser::parse(item.value);;
		return true;
	}
	catch (const Exception& ex) {
		logger().error("failed to parse value " + item.value + "as a int");
		logger().log(ex, __FILE__, __LINE__);
		return false;
	}
}

bool ZWaveMessage::extractBool(bool &value, const ZWaveSensorValue &item)
{
	if (item.value == "True") {
		value = true;
		return true;
	} else if (item.value == "False") {
		value = false;
		return true;
	}
	else {
		return false;
	}
}

bool ZWaveMessage::extractString(string &value,
	const ZWaveSensorValue &item)
{
	value = item.value;
	return true;
}

bool ZWaveMessage::extractSpecificValue(string &value, const int &commandClass,
	const int &index, const vector<ZWaveSensorValue> &zwaveValues)
{
	for (const ZWaveSensorValue &item : zwaveValues) {
		if (!isEqual(item, commandClass, index))
			continue;

		value = item.value;
		return true;
	}

	return false;
}

bool ZWaveMessage::parseTemperature(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	double temperature;

	for (const ZWaveSensorValue &item : zwaveValues) {
		if (!isEqual(item,
				COMMAND_CLASS_SENSOR_MULTILEVEL,
				SENSOR_INDEX_TEMPERATURE))
			continue;

		if (!extractFloat(temperature, item))
			continue;

		if (item.unit == "F")
			temperature = (temperature - 32)/1.8;

		value = to_string(temperature);
		return true;
	}

	return false;
}

bool ZWaveMessage::parseSwitchBinary(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	bool switchValue;

	for (const ZWaveSensorValue &item : zwaveValues) {
		if (!isEqual(item,
				COMMAND_CLASS_SWITCH_BINARY,
				SENSOR_INDEX_SENSOR))
			continue;

		if (!extractBool(switchValue, item))
			continue;

		value = to_string(switchValue);
		return true;
	}

	return false;
}

bool ZWaveMessage::parseBatteryLevel(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	return extractSpecificValue(value,
		COMMAND_CLASS_BATTERY,
		SENSOR_INDEX_BATTERY,
		zwaveValues);
}

bool ZWaveMessage::parseSensorValue(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	bool sensorValue;

	for (const ZWaveSensorValue &item : zwaveValues) {
		if (!isEqual(item,
				COMMAND_CLASS_SENSOR_BINARY,
				SENSOR_INDEX_SENSOR))
			continue;

		if (!extractBool(sensorValue, item))
			continue;

		value = to_string(sensorValue);
		return true;
	}

	return false;
}

bool ZWaveMessage::parseLuminance(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	return extractSpecificValue(value, COMMAND_CLASS_SENSOR_MULTILEVEL,
		SENSOR_INDEX_LUMINESCENCE, zwaveValues);
}

bool ZWaveMessage::parseHumidity(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	return extractSpecificValue(value, COMMAND_CLASS_SENSOR_MULTILEVEL,
		SENSOR_INDEX_HUMIDITY, zwaveValues);
}

bool ZWaveMessage::parseUltraviolet(string &value,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	return extractSpecificValue(value, COMMAND_CLASS_SENSOR_MULTILEVEL,
		SENSOR_INDEX_ULTRAVIOLET, zwaveValues);
}

void ZWaveMessage::sendActuatorValue(const OpenZWave::ValueID &valueId,
	const string &value)
{
	double data = 0;
	int valueType = valueId.GetType();

	try {
		switch(valueType) {
		case OpenZWave::ValueID::ValueType_Bool:
			OpenZWave::Manager::Get()->SetValue(valueId, asBool(value));
			break;
		case OpenZWave::ValueID::ValueType_Byte:
			data = NumberParser::parseFloat(value);
			OpenZWave::Manager::Get()->SetValue(valueId, uint8_t(data));
			break;
		case OpenZWave::ValueID::ValueType_Short:
			data = NumberParser::parseFloat(value);
			OpenZWave::Manager::Get()->SetValue(valueId, int16_t(data));
			break;
		case OpenZWave::ValueID::ValueType_Int:
			data = NumberParser::parseFloat(value);
			OpenZWave::Manager::Get()->SetValue(valueId, int(data));
			break;
		case OpenZWave::ValueID::ValueType_List:
			OpenZWave::Manager::Get()->SetValueListSelection(valueId, value);
			break;
		default:
			logger().error("Unsupported ValueID " + to_string(valueType),
					__FILE__, __LINE__);
			break;
		}
	} catch (const Exception &ex) {
		logger().error("failed to parse value " +
			to_string(data) + " as a float");
		logger().log(ex, __FILE__, __LINE__);
	}
}

bool ZWaveMessage::setActuator(const string &value, const int &commandClass,
	const int &index, const uint8_t &nodeId)
{
	FastMutex::ScopedLock guard(m_manager->lock());
	Nullable<NodeInfo> nodeInfo =
		m_manager->findNodeInfo(nodeId);

	if (nodeInfo.isNull())
		return false;

	for (const OpenZWave::ValueID &item : nodeInfo.value().values) {
		if (item.GetCommandClassId() == commandClass && item.GetIndex() == index) {
			logger().debug("Set actuator, commandClass " + to_string(commandClass)
					+ " index " + to_string(index));
			sendActuatorValue(item, value);
			return true;
		}
	}

	return false;
}

bool ZWaveMessage::isEqual(const ZWaveSensorValue &item,
	const int &commandClass, const int &index) const
{
	return item.commandClass == commandClass && index == item.index;
}

void ZWaveMessage::extractModuleTypes(list<ModuleType> &moduleTypes,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	string data;
	for (int i = 0; i <= MAXIMUN_SENSOR_TYPE; i++) {
		auto it = maps.find(i);
		if (it == maps.end())
			continue;

		if (!extractSpecificValue(data, COMMAND_CLASS_SENSOR_MULTILEVEL, i, zwaveValues))
			continue;

		moduleTypes.push_back(
			ModuleType(
				it->second, set<ModuleType::Attribute>())
		);
	}
}

void ZWaveMessage::extractSensorData(SensorData &sensorData,
	const vector<ZWaveSensorValue> &zwaveValues)
{
	string data;
	uint16_t modules = 0;

	for (int i = 0; i <= MAXIMUN_SENSOR_TYPE; i++) {
		auto it = maps.find(i);
		if (it == maps.end())
			continue;

		if (!extractSpecificValue(data, COMMAND_CLASS_SENSOR_MULTILEVEL, i, zwaveValues))
			continue;

		sensorData.insertValue(
			SensorValue(modules, NumberParser::parseFloat(data))
		);
		modules++;
	}
}

void ZWaveMessage::setManager(ZWaveDeviceManager *processor)
{
	m_manager = processor;
}
