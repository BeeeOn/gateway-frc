#include <Poco/RegularExpression.h>
#include <Poco/Thread.h>

#include "commands/DeviceAcceptCommand.h"
#include "commands/DeviceUnpairCommand.h"
#include "commands/GatewayListenCommand.h"
#include "commands/NewDeviceCommand.h"
#include "commands/ServerLastValueResult.h"
#include "core/CommandDispatcher.h"
#include "di/Injectable.h"
#include "io/AutoClose.h"
#include "jablotron/JablotronDevice.h"
#include "jablotron/JablotronDeviceAC88.h"
#include "jablotron/JablotronDeviceManager.h"
#include "udev/UDevEvent.h"

BEEEON_OBJECT_BEGIN(BeeeOn, JablotronDeviceManager)
BEEEON_OBJECT_CASTABLE(CommandHandler)
BEEEON_OBJECT_CASTABLE(StoppableRunnable)
BEEEON_OBJECT_CASTABLE(UDevListener)
BEEEON_OBJECT_REF("distributor", &JablotronDeviceManager::setDistributor)
BEEEON_OBJECT_REF("commandDispatcher", &JablotronDeviceManager::setCommandDispatcher)
BEEEON_OBJECT_END(BeeeOn, JablotronDeviceManager)

using namespace BeeeOn;
using namespace Poco;
using namespace std;

static const DevicePrefix PREFIX = DevicePrefix::PREFIX_JABLOTRON;
static const string JABLOTRON_VENDOR_ID = "0403";
static const string JABLOTRON_PRODUCT_ID = "6015";
static const int BAUD_RATE = 57600;
static const Timespan READ_TIMEOUT(1 * Timespan::SECONDS);
static const int MAX_DEVICES_IN_JABLOTRON = 32;
static const Timespan SLEEP_AFTER_FAILED(1 * Timespan::SECONDS);
static const int NUMBER_OF_RETRIES = 3;
static const int MAX_NUMBER_FAILED_REPEATS = 10;
static const Timespan DELAY_BEETWEEN_CYCLES(300 * Timespan::MILLISECONDS);
static const Timespan DELAY_AFTER_SET_SWITCH(1 * Timespan::SECONDS);
static string VENDOR_NAME = "Jablotron";

static const DeviceID DEFAULT_DEVICE_ID = DeviceID(PREFIX, 0);
static const int DEFAULT_PG_VALUE = 0;

JablotronDeviceManager::JablotronDeviceManager():
	DongleDeviceManager("JablotronDeviceManager", PREFIX),
	m_listenCallback(*this, &JablotronDeviceManager::stopListen),
	m_isListen(false),
	m_listenTimer(0, 0),
	m_pgx(DEFAULT_DEVICE_ID, DEFAULT_PG_VALUE),
	m_pgy(DEFAULT_DEVICE_ID, DEFAULT_PG_VALUE)
{
}

void JablotronDeviceManager::initJablotronDongle()
{
	m_serial.setBaudRate(BAUD_RATE);
	m_serial.setStopBits(SerialPort::StopBits::STOPBITS_1);
	m_serial.setParity(SerialPort::Parity::PARITY_NONE);
	m_serial.setDataBits(SerialPort::DataBits::DATABITS_8);
	m_serial.setNonBlocking(true);

	m_serial.setDevicePath(dongleName());
	m_serial.open();
	m_serial.flush();
}

void JablotronDeviceManager::dongleVersion()
{
	string message;

	m_serial.write("\x1BWHO AM I?\n");

	if (nextMessage(message) != MessageType::DATA) {
		logger().warning(
			"unexpected response: " + message,
			__FILE__, __LINE__);
		return;
	}

	if (wasTurrisDongleVersion(message)) {
		logger().information(
			"Jablotron Dongle version: "
			+ message,
		__FILE__, __LINE__);
	}
	else {
		logger().warning(
			"unknown dongle version: " + message,
		__FILE__, __LINE__);
	}
}

void JablotronDeviceManager::dongleAvailable()
{
	AutoClose<SerialPort> serial(m_serial);
	m_devices.clear();

	initJablotronDongle();
	dongleVersion();

	try {
		jablotronProcess();
	}
	catch (const IOException &ex) {
		// waiting so that UDevEvent can arrive after exception on serial port
		Thread::sleep(SLEEP_AFTER_FAILED.totalMilliseconds());
		ex.rethrow();
	}
}

void JablotronDeviceManager::jablotronProcess()
{
	string message;

	setupDongleDevices();
	loadDeviceList();

	while (!m_stop) {
		if (nextMessage(message) != MessageType::DATA) {
			logger().warning(
				"unexpected response: " + message,
				__FILE__, __LINE__);
			continue;
		}

		if (message.empty()) {
			logger().debug("empty message",
				__FILE__, __LINE__);
		}


		DeviceID id = JablotronDevice::buildID(extractSerialNumber(message));
		Mutex::ScopedLock guard(m_lock);

		auto it = m_devices.find(id);
		if (!it->second.isNull() && it->second->paired()) {
			shipMessage(message, it);
		}
		else if (m_isListen) {
			doNewDevice(id, it);
		}
		else {
			logger().debug(
				"device " + id.toString()
				+ " is not paired",
				__FILE__, __LINE__);
		}
	}
}

bool JablotronDeviceManager::accept(const Command::Ptr cmd)
{
	if (cmd->is<DeviceSetValueCommand>())
		return true;
	else if (cmd->is<GatewayListenCommand>())
		return true;
	else if (cmd->is<DeviceUnpairCommand>()
			|| cmd->is<DeviceSetValueCommand>()) {
		auto it = m_devices.find(cmd->cast<DeviceUnpairCommand>().deviceID());
		return it != m_devices.end();
	}
	else if (cmd->is<DeviceAcceptCommand>()) {
		return cmd->cast<DeviceAcceptCommand>().deviceID().prefix() == m_prefix;
	}

	return false;
}

void JablotronDeviceManager::handle(Command::Ptr cmd, Answer::Ptr answer)
{
	if (cmd->is<DeviceSetValueCommand>())
		doSetValueCommand(cmd.cast<DeviceSetValueCommand>(), answer);
	else if (cmd->is<GatewayListenCommand>())
		doListenCommand(cmd, answer);
	else if (cmd->is<DeviceUnpairCommand>())
		doUnpairCommand(cmd, answer);
	else if (cmd->is<DeviceAcceptCommand>())
		doDeviceAcceptCommand(cmd.cast<DeviceAcceptCommand>(), answer);
	else if (cmd->is<DeviceSetValueCommand>())
		doSetValueCommand(cmd.cast<DeviceSetValueCommand>(), answer);
}

void JablotronDeviceManager::doDeviceAcceptCommand(
	const DeviceAcceptCommand::Ptr &cmd, const Answer::Ptr &answer)
{
	Mutex::ScopedLock guard(m_lock);
	Result::Ptr result = new Result(answer);

	auto it = m_devices.find(cmd->deviceID());
	if (it == m_devices.end()) {
		logger().error(
			"unknown " + cmd->deviceID().toString()
			+ " device", __FILE__, __LINE__);

		result->setStatus(Result::FAILED);
	} else {
		it->second->setPaired(true);
		result->setStatus(Result::SUCCESS);
	}
}

void JablotronDeviceManager::doUnpairCommand(
	const Command::Ptr &cmd, const Answer::Ptr &answer)
{
	DeviceUnpairCommand::Ptr cmdUnpair = cmd.cast<DeviceUnpairCommand>();
	Result::Ptr result = new Result(answer);

	Mutex::ScopedLock guard(m_lock);
	auto it = m_devices.find(cmdUnpair->deviceID());

	if (it != m_devices.end()) {
		it->second = nullptr;
	}
	else {
		logger().warning(
			"attempt to unpair unknown device: "
			+ cmdUnpair->deviceID().toString());
	}

	result->setStatus(Result::SUCCESS);
}

void JablotronDeviceManager::doListenCommand(
	const Command::Ptr &cmd, const Answer::Ptr &answer)
{
	GatewayListenCommand::Ptr cmdListen = cmd.cast<GatewayListenCommand>();
	m_listenTimer.setStartInterval(cmdListen->duration().totalMilliseconds());

	m_isListen = true;
	m_listenTimer.start(m_listenCallback);

	Result::Ptr result = new Result(answer);
	result->setStatus(Result::SUCCESS);
}

void JablotronDeviceManager::stopListen(Timer &timer)
{
	logger().debug("listen is done", __FILE__, __LINE__);
	timer.restart(0);
	m_isListen = false;

	Mutex::ScopedLock guard(m_lock);
	for (auto device : m_devices) {
		if (device.second.isNull())
			continue;

		if (!device.second->paired())
			device.second = nullptr;
	}
}

void JablotronDeviceManager::doNewDevice(const DeviceID &deviceID,
	const map<DeviceID, SharedPtr<JablotronDevice>>::iterator &it)
{
	if (it->second.isNull()) {
		try {
			it->second = JablotronDevice::create(deviceID.ident());
		}
		catch (const InvalidArgumentException &ex) {
			logger().log(ex, __FILE__, __LINE__);
			return;
		}

		it->second->setPaired(false);
	}

	Answer::Ptr answer = new Answer(answerQueue());
	list<ModuleType> moduleTypes = it->second->moduleTypes();

	NewDeviceCommand::Ptr cmd =
		new NewDeviceCommand(
			it->second->deviceID(),
			VENDOR_NAME,
			it->second->name(),
			moduleTypes,
			it->second->refreshTime()
		);

	dispatch(cmd, answer);
}

void JablotronDeviceManager::loadDeviceList()
{
	set<DeviceID> deviceIDs = deviceList(-1);

	Mutex::ScopedLock guard(m_lock);
	for (auto &id : deviceIDs) {
		auto it = m_devices.find(id);

		if (it != m_devices.end()) {
			try {
				it->second = JablotronDevice::create(id.ident());
			}
			catch (const InvalidArgumentException &ex) {
				logger().log(ex, __FILE__, __LINE__);
				continue;
			}

			it->second->setPaired(true);
		}
	}

	obtainLastValue();
}

JablotronDeviceManager::MessageType JablotronDeviceManager::nextMessage(
			string &message)
{
	// find message between newline
	// message example: \nAC-88 RELAY:1\n
	const RegularExpression re("(?!\\n)[^\\n]*(?=\\n)");

	while (!re.extract(m_buffer, message) && !m_stop) {
		try {
			m_buffer += m_serial.read(READ_TIMEOUT);
		}
		catch (const TimeoutException &ex) {
			// avoid frozen state, allow to test m_stop time after time
			continue;
		}
	}

	// erase matched message from buffer + 2x newline
	m_buffer.erase(0, message.size() + 2);
	logger().debug("received data: " + message, __FILE__, __LINE__);

	return messageType(message);
}

JablotronDeviceManager::MessageType JablotronDeviceManager::messageType(
	const string &data)
{
	if (data == "OK")
		return MessageType::OK;
	else if (data == "ERROR")
		return MessageType::ERROR;

	return MessageType::DATA;
}

bool JablotronDeviceManager::wasTurrisDongleVersion(const std::string &message) const
{
	const RegularExpression re("TURRIS DONGLE V[0-9].[0-9]*");
	return re.match(message);
}

void JablotronDeviceManager::setupDongleDevices()
{
	string buffer;
	for (int i = 0; i < MAX_DEVICES_IN_JABLOTRON; i++) {
		Mutex::ScopedLock guard(m_lock);

		// we need 2-digits long value (zero-prefixed when needed)
		m_serial.write("\x1BGET SLOT:" + NumberFormatter::format0(i, 2) + "\n");

		if (nextMessage(buffer) != MessageType::DATA) {
			logger().error(
				"unknown message: " + buffer,
				__FILE__, __LINE__
			);

			continue;
		}

		try {
			uint32_t serialNumber = extractSerialNumber(buffer);
			DeviceID deviceID = JablotronDevice::buildID(serialNumber);

			// create empty JablotronDevice
			// after the obtain server device list, create JablotronDevice for paired devices
			m_devices.emplace(deviceID, nullptr);

			logger().debug(
				"created device: " + to_string(serialNumber)
				+ " from dongle", __FILE__, __LINE__);

			if (JablotronDevice::create(serialNumber).cast<JablotronDeviceAC88>().isNull())
				continue;

			if (m_pgx.first == DEFAULT_DEVICE_ID)
				m_pgx.first = deviceID;
			else
				m_pgy.first = deviceID;
		}
		catch (const InvalidArgumentException &ex) {
			logger().log(ex, __FILE__, __LINE__);
		}
		catch (const SyntaxException &ex) {
			logger().log(ex, __FILE__, __LINE__);
		}
	}
}

uint32_t JablotronDeviceManager::extractSerialNumber(const string &message)
{
	RegularExpression re("\\[([0-9]+)\\]");
	vector<string> tmp;

	if (re.split(message, tmp) == 2)
		return NumberParser::parseUnsigned(tmp[1]);

	throw InvalidArgumentException(
		"invalid serial number string: " + message);
}

void JablotronDeviceManager::shipMessage(const string &message,
	const map<DeviceID, SharedPtr<JablotronDevice>>::iterator &it)
{
	try {
		ship(it->second->extractSensorData(message));
	}
	catch (const RangeException &ex) {
		logger().error(
			"unexpected token index in message: " + message,
			__FILE__, __LINE__
		);
	}
	catch (const Exception &ex) {
		logger().log(ex, __FILE__, __LINE__);
	};
}

string JablotronDeviceManager::dongleMatch(const UDevEvent &e)
{
	const string &productID = e.properties()
		->getString("tty.ID_MODEL_ID", "");

	const string &vendorID = e.properties()
		->getString("tty.ID_VENDOR_ID", "");

	if (e.subsystem() == "tty") {
		if (vendorID == JABLOTRON_VENDOR_ID
				&& productID == JABLOTRON_PRODUCT_ID)
			return e.node();
	}

	return "";
}

/*
 * In case of packet retransmission, it is recommended to
 * retransfer packet 3 times with gap from 200 ms to 500 m
 */
bool JablotronDeviceManager::transmitMessage(const string &msg)
{
	int fails = 0;
	string message;

	for (int i = 0; i < NUMBER_OF_RETRIES
			&& fails < MAX_NUMBER_FAILED_REPEATS; i++) {
		if (!m_serial.write(msg)) {
			fails++;
			i--;
			continue;
		}

		if (nextMessage(message) == OK) {
			Thread::sleep(DELAY_AFTER_SET_SWITCH.totalMilliseconds());
			return true;
		}

		Thread::sleep(DELAY_BEETWEEN_CYCLES.totalMilliseconds());
	}

	Thread::sleep(DELAY_AFTER_SET_SWITCH.totalMilliseconds());
	return false;
}

bool JablotronDeviceManager::modifyValue(
			const DeviceID &deviceID, int value)
{
	Mutex::ScopedLock guard(m_lock);

	auto it = m_devices.find(deviceID);
	if (it == m_devices.end()) {
		throw InvalidArgumentException(
			"device " + it->second->deviceID().toString()
			+ " not found");
	}

	if (it->second.cast<JablotronDeviceAC88>().isNull()) {
		throw InvalidArgumentException(
			"device " + it->first.toString()
			+ " is not Jablotron AC-88");
	}

	if (!it->second->paired()) {
		InvalidArgumentException(
			"device " + it->first.toString()
			+ " is not paired");
	}

	if (m_pgx.first == deviceID)
		m_pgx.second = value;
	else
		m_pgy.second = value;

	string modifyString = "\x1BTX ENROLL:0 PGX:" + to_string(m_pgx.second) +
		" PGY:" + to_string(m_pgy.second) + " ALARM:0 BEEP:FAST\n";

	return transmitMessage(modifyString);
}

void JablotronDeviceManager::obtainLastValue()
{
	int value;
	for (auto &device : m_devices) {
		if (device.second.cast<JablotronDeviceAC88>().isNull()
				|| !device.second->paired()) {
			continue;
		}

		try {
			value = lastValue(device.first, 0);
		}
		catch (const Exception &ex) {
			logger().log(ex, __FILE__, __LINE__);
			continue;
		}

		if (!modifyValue(device.first, value)) {
			logger().warning(
				"last value on device: " + device.first.toString()
				+ " is not set",
				__FILE__, __LINE__);
		}
	}
}

void JablotronDeviceManager::doSetValueCommand(
	DeviceSetValueCommand::Ptr cmd, Answer::Ptr answer)
{
	Result::Ptr result = new Result(answer);
	bool ret;

	try {
		ret = modifyValue(cmd->deviceID(), cmd->value());
	}
	catch (const Exception &ex) {
		logger().log(ex, __FILE__, __LINE__);

		result->setStatus(Result::FAILED);
		return;
	}

	if (ret)
		result->setStatus(Result::SUCCESS);
	else
		result->setStatus(Result::FAILED);
}
