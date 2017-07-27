#pragma once

#include <string>

#include <openzwave/Manager.h>

namespace BeeeOn {

class ZWaveDriver {
public:
	ZWaveDriver(const std::string &driver = "");

	void setDriverPath(const std::string &driverPath);

	/**
	 * Get USB driver path
	 * @return USB driver path
	 */
	std::string driverPath();

	/**
	 * Checks if it is USB device. The USB device is
	 * device which contains "usb" string in path.
	 * @return true if it is USB device
	 */
	bool isUSBDriver() const;

	/**
	 * Address a new driver for a Z-Wave controller.
	 * @return True if driver successfully added
	 */
	bool registerItself();

	/**
	 * Removes the driver for a Z-Wave controller.
	 * and closes the controller.
	 * @return True if the driver was removed, false if it could not be found
	 */
	bool unregisterItself();

private:
	std::string m_driver;
};

}
