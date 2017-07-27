#include <Poco/RegularExpression.h>

#include "z-wave/ZWaveDriver.h"

using namespace BeeeOn;
using namespace OpenZWave;
using namespace Poco;
using namespace std;

ZWaveDriver::ZWaveDriver(const string &driver) :
	m_driver(driver)
{
}

void ZWaveDriver::setDriverPath(const string &driverPath)
{
	m_driver = driverPath;
}

string ZWaveDriver::driverPath()
{
	return m_driver;
}

bool ZWaveDriver::isUSBDriver() const
{
	const RegularExpression re("usb");

	return re.match(m_driver);
}

bool ZWaveDriver::registerItself()
{
	if (isUSBDriver())
		return Manager::Get()->AddDriver("HID Controller",
			Driver::ControllerInterface_Hid);
	else
		return Manager::Get()->AddDriver(m_driver);
}

bool ZWaveDriver::unregisterItself()
{
	if (isUSBDriver())
		return Manager::Get()->RemoveDriver("HID Controller");
	else
		return Manager::Get()->RemoveDriver(m_driver);
}
