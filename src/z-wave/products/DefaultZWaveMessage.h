#pragma once

#include "z-wave/ZWaveMessage.h"

namespace BeeeOn {

class DefaultZWaveMessage : public ZWaveMessage {
public:
	SensorData extractValues(
		const std::vector<ZWaveSensorValue> &zwaveValues) override;

	std::list<ModuleType> moduleTypes(
		const std::vector<ZWaveSensorValue> &zwaveValues) override;
};

}
