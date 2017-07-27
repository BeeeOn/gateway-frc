#pragma once

#include <map>
#include <set>

#include <Poco/Mutex.h>
#include <Poco/SharedPtr.h>
#include <Poco/Timer.h>

#include "commands/DeviceSetValueCommand.h"
#include "core/DongleDeviceManager.h"
#include "io/SerialPort.h"
#include "jablotron/JablotronDevice.h"
#include "model/DeviceID.h"

namespace BeeeOn {

class JablotronDevice;

/**
 * JablotronDeviceManager manages a cache of devices associated with the
 * Jablotron dongle and paired on the server. Devices associated with the
 * dongle are not necessarily those paired by the user. Thus, the
 * associated devices are managed in the form <DeviceID, NULL>.
 *
 * When a device is paired by its user, the JablotronDeviceManager creates
 * a new instance of a JablotronDevice and manages such device as
 * <DeviceID, SharedPtr<JablotronDevice>>.
 *
 * Physical Jablotron devices can be associated with the dongle only
 * in a software-based way. Thus, they are associated by an instance
 * of the JablotronDeviceManager (and we know about it) or the dongle
 * must be physically connected to another device. Every time a dongle
 * is physical reconnected, the JablotronDeviceManager checks the
 * associated devices and the coherency of the device list thus maintained.
 *
 * However, if another process (by mistake or even intentionally) performs
 * a device reassociation with the dongle directly via the serial port
 * in parallel while the JablotronDeviceManaher is running, then the
 * JablotronDeviceManager could behave in an unexpected way. Such
 * behaviour is undefined.
 */
class JablotronDeviceManager : public DongleDeviceManager {
public:
	JablotronDeviceManager();

	bool accept(const Command::Ptr cmd) override;
	void handle(Command::Ptr cmd, Answer::Ptr answer) override;

	std::string dongleMatch(const UDevEvent &e) override;
	void dongleAvailable() override;

private:
	enum MessageType {
		OK,
		ERROR,
		DATA,
	};

	/**
	 * Load device list from server and create JablotronDevice.
	 */
	void loadDeviceList();

	/**
	 * It shows Jablotron Turris Dongle firmware version.
	 * Example: TURRIS DONGLE V1.4
	 */
	void dongleVersion();
	bool wasTurrisDongleVersion(const std::string &message) const;

	void initJablotronDongle();
	void jablotronProcess();

	/**
	 * It picks next message from buffer and removes picked message from it.
	 * If buffer is empty, method reads data from serial into the buffer.
	 */
	MessageType nextMessage(std::string &message);

	/**
	 * Type of received message. If message contains data from sensor,
	 * message type is DATA. If message contains return value from sent
	 * message, message type is OK/ERROR.
	 */
	MessageType messageType(const std::string &data);

	/**
	 * Extract serial number from string.
	 * Example of string: SLOT:01 [09491911]
	 */
	uint32_t extractSerialNumber(const std::string &message);

	/**
	 * Load devices from USB Dongle.
	 */
	void setupDongleDevices();

	/**
	 * It extracts data from message based on DeviceID
	 * and sends message to distributor.
	 */
	void shipMessage(const std::string &message, const std::map<DeviceID,
		Poco::SharedPtr<JablotronDevice>>::iterator &it);

	/**
	 * It sends message to Jablotron dongle and checks if sent
	 * message is accepted.
	 *
	 * @return True if Turris dongle accepts message.
	 */
	bool transmitMessage(const std::string &msg);

	/**
	 * It obtains last value from server and sets value to the device.
	 */
	void obtainLastValue();

	/**
	 * It modifies value in the device.
	 */
	bool modifyValue(const DeviceID &deviceID, int value);

	void stopListen(Poco::Timer &timer);

	void doSetValueCommand(DeviceSetValueCommand::Ptr cmd, Answer::Ptr answer);
	void doListenCommand(const Command::Ptr &cmd, const Answer::Ptr &answer);
	void doNewDevice(const DeviceID &deviceID, const std::map<DeviceID,
		Poco::SharedPtr<JablotronDevice>>::iterator &it);
	void doUnpairCommand(const Command::Ptr &cmd, const Answer::Ptr &answer);
	void doDeviceAcceptCommand(const DeviceAcceptCommand::Ptr &cmd, const Answer::Ptr &answer);

private:
	SerialPort m_serial;
	Poco::Mutex m_lock;
	std::map<DeviceID, Poco::SharedPtr<JablotronDevice>> m_devices;
	std::string m_buffer;

	Poco::TimerCallback<JablotronDeviceManager> m_listenCallback;
	Poco::AtomicCounter m_isListen;
	Poco::Timer m_listenTimer;

	/**
	 * Field X - output X
	 * Slot which contains the state of the first AC-88 device
	 */
	std::pair<DeviceID, int> m_pgx;

	/**
	 * Field Y - output Y
	 * Slot which contains the state of the second AC-88 device
	 */
	std::pair<DeviceID, int> m_pgy;
};

}
