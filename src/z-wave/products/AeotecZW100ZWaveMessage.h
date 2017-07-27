#pragma once

#include <map>

#include "z-wave/ZWaveMessage.h"

namespace BeeeOn {

const static int AEOTEC_ZW100 = 0x0064;

class AeotecZW100ZWaveMessage : public ZWaveMessage {
public:
	AeotecZW100ZWaveMessage();

	SensorData extractValues(
		const std::vector<ZWaveSensorValue> &zwaveValues) override;

	bool modifyValue(const ModuleID &moduleID, double value,
		const uint8_t nodeId) override;

	void modifyValueAfterStart() override;

	std::list<ModuleType> moduleTypes(
		const std::vector<ZWaveSensorValue> &zwaveValues) override;

private:
	std::map<std::string, std::string> m_pirSensor;

	void setMapValue();

	bool extractPirSensitivity(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	bool extractRefreshTime(std::string &value,
		const std::vector<ZWaveSensorValue> &zwaveValues);

	void extractDetectionSensor(SensorData &sensorData,
		const std::vector<ZWaveSensorValue> &zwaveValues);
};

}
