#include "DevicePrefix.h"

using namespace BeeeOn;

EnumHelper<DevicePrefixEnum::Raw>::ValueMap
&DevicePrefixEnum::valueMap()
{
	static EnumHelper<DevicePrefixEnum::Raw>::ValueMap valueMap = {
		{DevicePrefixEnum::PREFIX_INVALID, "Invalid"},
		{DevicePrefixEnum::PREFIX_FITPROTOCOL, "Fitprotocol"},
		{DevicePrefixEnum::PREFIX_PRESSURE_SENSOR, "PressureSensor"},
		{DevicePrefixEnum::PREFIX_VIRTUAL_DEVICE, "VirtualDevice"},
		{DevicePrefixEnum::PREFIX_VPT, "VPT"},
		{DevicePrefixEnum::PREFIX_OPENHAB, "OpenHAB"},
		{DevicePrefixEnum::PREFIX_BLUETOOTH, "Bluetooth"},
		{DevicePrefixEnum::PREFIX_BELKIN_WEMO, "BelkinWemo"},
		{DevicePrefixEnum::PREFIX_ZWAVE, "Z-Wave"},
		{DevicePrefixEnum::PREFIX_JABLOTRON, "Jablotron"},
		{DevicePrefixEnum::PREFIX_IQRF, "IQRF"},
	};

	return valueMap;
}
