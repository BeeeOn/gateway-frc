#ifndef BEEEON_DEVICE_PREFIX_H
#define BEEEON_DEVICE_PREFIX_H

#include "util/Enum.h"

namespace BeeeOn {

/*
 * The prefix represents the identification of the device group
 * with the same specifications.
 */
class DevicePrefixEnum {
public:
	enum Raw {
		PREFIX_INVALID = 0x00,
		PREFIX_FITPROTOCOL = 0xa1,
		PREFIX_PRESSURE_SENSOR = 0xa2,
		PREFIX_VIRTUAL_DEVICE = 0xa3,
		PREFIX_VPT = 0xa4,
		PREFIX_OPENHAB = 0xa5,
		PREFIX_BLUETOOTH = 0xa6,
		PREFIX_BELKIN_WEMO = 0xa7,
		PREFIX_ZWAVE = 0xa8,
		PREFIX_JABLOTRON = 0x09,
		PREFIX_IQRF = 0xa9,
	};

	static EnumHelper<Raw>::ValueMap &valueMap();
};

typedef Enum<DevicePrefixEnum> DevicePrefix;


}
#endif
