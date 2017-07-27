#pragma once

#include <string>
#include <vector>

#include <Poco/AutoPtr.h>
#include <Poco/SharedPtr.h>
#include <Poco/RefCountedObject.h>

#include <openzwave/Manager.h>

#include "model/ModuleType.h"
#include "model/SensorData.h"
#include "util/Loggable.h"

namespace BeeeOn {

class ZWaveDeviceManager;

const static int COMMAND_CLASS_ALARM = 113;
const static int COMMAND_CLASS_BATTERY = 128;
const static int COMMAND_CLASS_BULGAR = 10;
const static int COMMAND_CLASS_CONFIGURATION = 112;
const static int COMMAND_CLASS_SENSOR_BINARY = 48;
const static int COMMAND_CLASS_SENSOR_MULTILEVEL = 49;
const static int COMMAND_CLASS_SWITCH_BINARY = 37;

const static int SENSOR_INDEX_BULGAR = 10;

/**
 * Represents a single value from the Z-Wave network.
 */
struct ZWaveSensorValue {
	int commandClass;
	int index;
	OpenZWave::ValueID valueID;
	std::string value;
	std::string unit;
};

/**
 * Reports from products containing methods for processing
 * values from the network or to adjust values.
 */
class ZWaveMessage : public Poco::RefCountedObject, public Loggable {
public:
	typedef Poco::AutoPtr<ZWaveMessage> Ptr;

	/**
	 * Extract data from ZWaveSensorValue struct and parse to SensorData.
	 * It convert from command class and index to module id.
	 * @param zwaveValues Values from Z-Wave network
	 * @return extracted data from ZWaveSensorValue
	 */
	virtual SensorData extractValues(
		const std::vector<ZWaveSensorValue> &zwaveValues) = 0;

	/**
	 * It sets data to Z-Wave device.
	 * @param beeeOnValues Data from adapter which sets for Z-Wave device
	 */
	virtual bool modifyValue(const ModuleID &moduleID, double value,
		const uint8_t nodeId);

	/**
	 * Setting specific sensor data after start device.
	 */
	virtual void modifyValueAfterStart();

	virtual std::list<ModuleType> moduleTypes(
		const std::vector<ZWaveSensorValue> &zwaveValues) = 0;

	void setManager(ZWaveDeviceManager *manager);

	virtual ~ZWaveMessage()
	{
	}

protected:
	/**
	 * Extract type of measured values from receive of data.
	 */
	void extractModuleTypes(std::list<ModuleType> &moduleTypes,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Extract measured values from receive of data.
	 */
	void extractSensorData(SensorData &sensorData,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	bool asBool(const std::string &value);

	/**
	 * Check If equal.
	 * @param commandClass1 info reports from ZWave network
	 * @param commandClass info reports from ZWave network
	 * @param index in command class info reports from ZWave network
	 * @param index in command class info reports from ZWave network
	 * @return true if it equals
	 */
	bool isEqual(const ZWaveSensorValue &item, const int &commandClass,
		const int &index) const;

	/**
	 * Parse float number from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value parsed message
	 * @param item zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool extractFloat(double &value, const ZWaveSensorValue &item);

	/**
	 * Parse int number from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value parsed message
	 * @param item zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool extractInt(int &value, const ZWaveSensorValue &item);

	/**
	 * Parse bool value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value parsed message
	 * @param item zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool extractBool(bool &value, const ZWaveSensorValue &item);

	/**
	 * Parse string from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value parsed message
	 * @param item zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool extractString(std::string &value, const ZWaveSensorValue &item);

	/**
	 * Parse specific sensor value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool extractSpecificValue(std::string &value, const int &commandClass,
		const int &index, const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse battery level value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseBatteryLevel(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse sensor value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseSensorValue(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse temperature value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseTemperature(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse switch binary value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseSwitchBinary(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse luminance value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseLuminance(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse humidity value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseHumidity(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Parse ultraviolet value from ZWaveSensorValue by command class and index.
	 * ZWaveSensorValue contains value from ZWave network for specific device.
	 * @param value sensor value
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param zwaveValues zwave sensor value vector
	 * @return true if it is was parsed successfully
	 */
	bool parseUltraviolet(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	/**
	 * Send data to ZWave network for setting ZWave device. It need to find out
	 * type of value which need to sent.
	 * @param valueId it provides a unique ID for a value reported by a ZWave device
	 * @param value it is value which contains data to setting
	 */
	void sendActuatorValue(const OpenZWave::ValueID &valueId,
		const std::string &value);

	/**
	 * Find value ID to be sets and send to ZWave netwrok.
	 * @param value it is value to be sets
	 * @param commandClass info reports from ZWave network
	 * @param index index in command class info reports from ZWave network
	 * @param nodeId unique identifier for device in ZWave network
	 */
	bool setActuator(const std::string &value, const int &commandClass,
		const int &index, const uint8_t &nodeId);

protected:
	ZWaveDeviceManager *m_manager;
};

}
