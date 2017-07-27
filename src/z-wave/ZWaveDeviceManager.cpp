#include <list>
#include <map>

#include <Poco/Logger.h>

#include <openzwave/Options.h>
#include <openzwave/Manager.h>
#include <commands/DeviceUnpairCommand.h>

#include "commands/DeviceSetValueCommand.h"
#include "commands/GatewayListenCommand.h"
#include "core/CommandDispatcher.h"
#include "di/Injectable.h"
#include "udev/UDevEvent.h"
#include "z-wave/GenericZWaveMessageFactory.h"
#include "z-wave/ZWaveDeviceManager.h"
#include "z-wave/ZWavePocoLoggerAdapter.h"

BEEEON_OBJECT_BEGIN(BeeeOn, ZWaveDeviceManager)
BEEEON_OBJECT_CASTABLE(CommandHandler)
BEEEON_OBJECT_CASTABLE(StoppableRunnable)
BEEEON_OBJECT_CASTABLE(UDevListener)
BEEEON_OBJECT_TEXT("userPath", &ZWaveDeviceManager::setUserPath)
BEEEON_OBJECT_TEXT("configPath", &ZWaveDeviceManager::setConfigPath)
BEEEON_OBJECT_NUMBER("pollInterval", &ZWaveDeviceManager::setPollInterval)
BEEEON_OBJECT_REF("gatewayInfo", &ZWaveDeviceManager::setGatewayInfo)
BEEEON_OBJECT_REF("commandDispatcher", &ZWaveDeviceManager::setCommandDispatcher)
BEEEON_OBJECT_REF("distributor", &ZWaveDeviceManager::setDistributor)
BEEEON_OBJECT_REF("messageFactory", &ZWaveDeviceManager::setMessageFactory)
BEEEON_OBJECT_HOOK("done", &ZWaveDeviceManager::installConfiguration);
BEEEON_OBJECT_END(BeeeOn, ZWaveDeviceManager)

using namespace BeeeOn;
using namespace Poco;
using namespace std;

const static DevicePrefix PREFIX = DevicePrefix::PREFIX_ZWAVE;
const static string ZWAVE_SERIAL_ID = "0658";
static const int NODE_ID_MASK = 0xff;
static const int DEFAULT_UNPAIR_TIMEOUT_MS = 10000;

static void onNotification(
	OpenZWave::Notification const *notification, void *context)
{
	ZWaveDeviceManager *processor = (ZWaveDeviceManager *) context;
	processor->onNotification(notification);
}

ZWaveDeviceManager::ZWaveDeviceManager():
	DeviceManager("ZWaveDeviceManager", PREFIX),
	m_state(State::IDLE),
	m_listenCallback(*this, &ZWaveDeviceManager::stopListen),
	m_isListen(false),
	m_listenTimer(0, 0),
	m_callbackUnpair(*this, &ZWaveDeviceManager::stopUnpair),
	m_derefUnpair(DEFAULT_UNPAIR_TIMEOUT_MS, 0)
{
}

ZWaveDeviceManager::~ZWaveDeviceManager()
{
	OpenZWave::Manager::Get()->AddWatcher(::onNotification, this);

	OpenZWave::Manager::Destroy();
	OpenZWave::Options::Destroy();
}

void ZWaveDeviceManager::installConfiguration()
{
	OpenZWave::Options::Create(m_configPath, m_userPath, "");
	OpenZWave::Options::Get()->AddOptionInt("PollInterval", m_pollInterval);
	OpenZWave::Options::Get()->AddOptionBool("logging", true);
	OpenZWave::Options::Get()->AddOptionBool("SaveConfiguration", false);
	OpenZWave::Options::Get()->Lock();

	OpenZWave::Manager::Create();

	// pocoLogger is deleted by OpenZWave library
	// pocoLogger must be set after Manager::Create()
	ZWavePocoLoggerAdapter *pocoLogger =
		new ZWavePocoLoggerAdapter(Loggable::forMethod("OPenZWaveLibrary"));
	OpenZWave::Log::SetLoggingClass(pocoLogger);

	OpenZWave::Manager::Get()->AddWatcher(::onNotification, this);
}

void ZWaveDeviceManager::run()
{
	/**
	 * TODO: tento while je len kvoli tomu, aby po dobu behu programu
	 * existovalo vlakno a bolo mozne na neho zavolat metodu stop.
	 * Aktualne je problem, ze niektore referencie sa nerusia a tym padom
	 * sa nevola destruktor.
	 */
	while(!m_stop)
		Poco::Thread::sleep(10000);
}

void ZWaveDeviceManager::stop()
{
	m_driver.unregisterItself();
	OpenZWave::Manager::Get()->WriteConfig(m_homeID);
	DeviceManager::stop();
}

string ZWaveDeviceManager::dongleMatch(const UDevEvent &e)
{
	const string &serial = e.properties()
		->getString("tty.ID_VENDOR");

	if (e.subsystem() == "tty") {
		if (serial == ZWAVE_SERIAL_ID)
			return e.node();
	}

	return "";
}

void ZWaveDeviceManager::onAdd(const UDevEvent &event)
{
	FastMutex::ScopedLock guard(m_lock);

	if (!m_dongleName.empty()) {
		logger().trace("ignored event " + event.toString(),
			__FILE__, __LINE__);
		return;
	}

	const string &name = dongleMatch(event);
	if (name.empty()) {
		logger().trace("event " + event.toString() + " does not match",
			__FILE__, __LINE__);
		return;
	}

	logger().debug("registering dongle " + event.toString(),
		__FILE__, __LINE__);

	m_state = State::IDLE;
	m_dongleName = name;

	m_driver.setDriverPath(m_dongleName);
	m_driver.registerItself();
}

void ZWaveDeviceManager::onRemove(const UDevEvent &event)
{
	FastMutex::ScopedLock guard(m_lock);

	if (m_dongleName.empty()) {
		logger().trace("ignored event " + event.toString(),
			__FILE__, __LINE__);
		return;
	}

	const string &name = dongleMatch(event);
	if (name.empty()) {
		logger().trace("event " + event.toString() + " does not match",
			__FILE__, __LINE__);
		return;
	}

	logger().debug("unregistering dongle " + event.toString(),
		__FILE__, __LINE__);

	m_state = State::IDLE;

	m_dongleName.clear();
	m_driver.unregisterItself();
}

bool ZWaveDeviceManager::accept(const Command::Ptr cmd)
{
	if (cmd->is<GatewayListenCommand>())
		return true;
	else if (cmd->is<DeviceSetValueCommand>()
			|| cmd->is<DeviceUnpairCommand>()) {
			uint8_t nodeID = cmd->cast<DeviceSetValueCommand>().deviceID();

			return m_nodesMap.find(nodeID) != m_nodesMap.end();
	}

	return false;
}

void ZWaveDeviceManager::handle(Command::Ptr cmd, Answer::Ptr answer)
{
	if (cmd->is<GatewayListenCommand>())
		doListenCommand(cmd, answer);
	else if (cmd->is<DeviceSetValueCommand>())
		doSetValueCommand(cmd, answer);
	else if (cmd->is<DeviceUnpairCommand>())
		doUnpairCommand(cmd, answer);
}

void ZWaveDeviceManager::doListenCommand(
	const Command::Ptr &cmd, const Answer::Ptr &answer)
{
	GatewayListenCommand::Ptr cmdListen = cmd.cast<GatewayListenCommand>();
	Result::Ptr result = new Result(answer);

	if (m_state != State::NODE_QUERIED) {
		result->setStatus(Result::FAILED);
		return;
	}

	m_listenTimer.setStartInterval(cmdListen->duration().totalMilliseconds());
	m_listenTimer.start(m_listenCallback);

	m_isListen = true;
	OpenZWave::Manager::Get()->AddNode(m_homeID);
	result->setStatus(Result::SUCCESS);
}

void ZWaveDeviceManager::stopUnpair(Timer &timer)
{
	OpenZWave::Manager::Get()->CancelControllerCommand(m_homeID);
	timer.restart(0);

	logger().debug("unpair is done", __FILE__, __LINE__);
}

void ZWaveDeviceManager::doUnpairCommand(
	const Command::Ptr &cmd, const Answer::Ptr &answer)
{
	Result::Ptr result = new Result(answer);
	DeviceUnpairCommand::Ptr cmdUnpair = cmd.cast<DeviceUnpairCommand>();

	uint8_t nodeID = nodeIDFromDeviceID(cmdUnpair->deviceID());
	auto it = m_nodesMap.find(nodeID);
	if (it == m_nodesMap.end()) {
		result->setStatus(Result::SUCCESS);
		return;
	}

	m_nodesMap.erase(nodeID);
	result->setStatus(Result::SUCCESS);

	OpenZWave::Manager::Get()->RemoveNode(m_homeID);
	m_listenTimer.start(m_listenCallback);
}

void ZWaveDeviceManager::stopListen(Timer &timer)
{
	OpenZWave::Manager::Get()->CancelControllerCommand(m_homeID);
	timer.restart(0);

	logger().debug("listen is done", __FILE__, __LINE__);
	m_isListen = false;
}

void ZWaveDeviceManager::doSetValueCommand(
	const Command::Ptr &cmd, const Answer::Ptr &answer)
{
	Result::Ptr result = new Result(answer);
	DeviceSetValueCommand::Ptr cmdSet = cmd.cast<DeviceSetValueCommand>();

	uint32_t manufacturer;
	uint32_t product;
	uint8_t nodeId = nodeIDFromDeviceID(cmdSet->deviceID());

	try {
		manufacturer = manufacturerID(nodeId);
		product = productID(nodeId);
	}
	catch (const InvalidArgumentException &ex) {
		logger().log(ex, __FILE__, __LINE__);
		result->setStatus(Result::FAILED);
		return;
	}

	ZWaveMessage::Ptr message = m_factory->create(manufacturer, product);
	message->setManager(this);
	if (message->modifyValue(cmdSet->moduleID(), cmdSet->value(), nodeId))
		result->setStatus(Result::SUCCESS);
	else
		result->setStatus(Result::FAILED);
}

Nullable<NodeInfo> ZWaveDeviceManager::findNodeInfo(
	const uint8 nodeId)
{
	Nullable<NodeInfo> nullable;

	auto search = m_nodesMap.find(nodeId);
	if (search != m_nodesMap.end()) {
		NodeInfo &nodeInfo = search->second;
		nullable = nodeInfo;
	}

	return nullable;
}

void ZWaveDeviceManager::valueAdded(
	const OpenZWave::Notification *notification)
{
	auto it = m_nodesMap.find(notification->GetNodeId());
	if (it == m_nodesMap.end())
		return;

	it->second.values.push_back(notification->GetValueID());
}

uint32_t ZWaveDeviceManager::manufacturerID(uint8_t nodeId) const
{
	string manufacturerString =
		OpenZWave::Manager::Get()->GetNodeManufacturerId(m_homeID, nodeId);

	try {
		return NumberParser::parseHex(manufacturerString);
	}
	catch (const Poco::SyntaxException &ex) {
		throw Poco::InvalidArgumentException(
			"failed to parse manufacturer id: " + manufacturerString);
	}
}

uint32_t ZWaveDeviceManager::productID(uint8_t nodeId) const
{
	string productString =
		OpenZWave::Manager::Get()->GetNodeProductId(m_homeID, nodeId);

	try {
		return NumberParser::parseHex(productString);
	}
	catch (const Poco::SyntaxException &ex) {
		throw Poco::InvalidArgumentException(
			"failed to parse product id: " + productString);
	}
}

void ZWaveDeviceManager::valueChanged(
	const OpenZWave::Notification *notification)
{
	uint8_t nodeId = notification->GetNodeId();

	auto it = m_nodesMap.find(nodeId);
	if (it == m_nodesMap.end()) {
		logger().error(
			"unknown ZWave device nodeID: " + to_string(nodeId),
			__FILE__, __LINE__
		);
		return;
	}

	if (!it->second.paired) {
		logger().debug(
			"device: " + buildID(nodeId).toString()
			+ " is not paired", __FILE__, __LINE__);
		return;
	}

	try {
		sendValue(nodeId, it->second.values);
	}
	catch (const Poco::Exception &ex) {
		logger().log(ex, __FILE__, __LINE__);
	}
}

void ZWaveDeviceManager::sendValue(
	const uint8_t nodeId, const list<OpenZWave::ValueID> &values)
{
	SensorData sensorData;
	vector<ZWaveSensorValue> zwaveValues;

	ZWaveMessage::Ptr message =
		m_factory->create(
			manufacturerID(nodeId),
			productID(nodeId)
		);

	for (auto &item : values) {
		string value;
		OpenZWave::Manager::Get()->GetValueAsString(item, &value);

		zwaveValues.push_back({
			item.GetCommandClassId(),
			item.GetIndex(),
			item,
			value,
			OpenZWave::Manager::Get()->GetValueUnits(item)});
	}

	sensorData = message->extractValues(zwaveValues);
	sensorData.setDeviceID(buildID(nodeId));
	ship(sensorData);
}

void ZWaveDeviceManager::valueRemoved(
	const OpenZWave::Notification *notification)
{
	auto it = m_nodesMap.find(notification->GetNodeId());
	if (it == m_nodesMap.end())
		return;

	m_nodesMap.erase(it);
}

void ZWaveDeviceManager::nodeAdded(
	const OpenZWave::Notification *notification)
{
	NodeInfo nodeInfo;
	nodeInfo.polled = false;
	nodeInfo.paired = false;
	m_nodesMap.emplace(pair<uint8_t, NodeInfo>(notification->GetNodeId(), nodeInfo));

	OpenZWave::Manager::Get()->CancelControllerCommand(notification->GetHomeId());
	OpenZWave::Manager::Get()->WriteConfig(m_homeID);
}

void ZWaveDeviceManager::nodeRemoved(
	const OpenZWave::Notification *notification)
{
	uint8_t nodeId = notification->GetNodeId();
	auto it = m_nodesMap.find(nodeId);

	if (it != m_nodesMap.end())
		m_nodesMap.erase(nodeId);

	OpenZWave::Manager::Get()->WriteConfig(m_homeID);
}

void ZWaveDeviceManager::onNotification(
	const OpenZWave::Notification *notification)
{
	//FastMutex::ScopedLock guard(m_lock);
	auto it = m_nodesMap.find(notification->GetNodeId());

	switch (notification->GetType()) {
	case OpenZWave::Notification::Type_ValueAdded:
		valueAdded(notification);
		break;
	case OpenZWave::Notification::Type_ValueRemoved:
		valueRemoved(notification);
		break;
	case OpenZWave::Notification::Type_ValueChanged:
		valueChanged(notification);
		break;
	case OpenZWave::Notification::Type_NodeAdded:
		nodeAdded(notification);
		break;
	case OpenZWave::Notification::Type_NodeRemoved:
		nodeRemoved(notification);
		break;
	case OpenZWave::Notification::Type_NodeEvent: {
		break;
	}
	case OpenZWave::Notification::Type_PollingDisabled: {
		if (it != m_nodesMap.end())
			it->second.polled = false;

		break;
	}
	case OpenZWave::Notification::Type_PollingEnabled: {
		if (it != m_nodesMap.end())
			it->second.polled = true;

		break;
	}
	case OpenZWave::Notification::Type_DriverReady: {
		m_homeID = notification->GetHomeId();
		OpenZWave::Manager::Get()->WriteConfig(m_homeID);
		m_state = USB_READY;

		logger().debug(
			"usb driver " + m_dongleName + " ready "
			+ "with homeID: " + NumberFormatter::formatHex(m_homeID, true),

			__FILE__, __LINE__);
		break;
	}
	case OpenZWave::Notification::Type_DriverFailed: {
		m_state = State::IDLE;

		logger().debug(
			"usb driver " + m_dongleName + "failed",
			__FILE__, __LINE__);
		break;
	}
	case OpenZWave::Notification::Type_ValueRefreshed:
		break;
	case OpenZWave::Notification::Type_AwakeNodesQueried:
	case OpenZWave::Notification::Type_AllNodesQueried:
	case OpenZWave::Notification::Type_AllNodesQueriedSomeDead:
		m_state = NODE_QUERIED;

		logger().debug(
			"all nodes queuried",
			__FILE__, __LINE__);

		loadDeviceList();
		break;
	case OpenZWave::Notification::Type_DriverReset:
	case OpenZWave::Notification::Type_Notification:
	case OpenZWave::Notification::Type_NodeNaming:
	case OpenZWave::Notification::Type_NodeProtocolInfo:
	case OpenZWave::Notification::Type_NodeQueriesComplete:
	default:
		break;
	}
}

DeviceID ZWaveDeviceManager::buildID(uint8_t nodeID)
{
	uint64_t deviceID = 0;
	uint64_t homeId64 = m_homeID;
	uint64_t gatewayId64 = 0; // TODO m_gatewayInfo->gatewayID();

	// | 8b | 16b | 32b | 8b |
	// prefix, gatewayID, homeId, nodeId
	deviceID = uint64_t(DevicePrefix::PREFIX_ZWAVE) << 56;
	deviceID |= gatewayId64 << 40;
	deviceID |= homeId64 << 8;
	deviceID |= nodeID;

	return DeviceID(deviceID);
}

FastMutex &ZWaveDeviceManager::lock()
{
	return m_lock;
}

void ZWaveDeviceManager::setUserPath(const string &userPath)
{
	m_userPath = userPath;
}

void ZWaveDeviceManager::setConfigPath(const string &configPath)
{
	m_configPath = configPath;
}

void ZWaveDeviceManager::setPollInterval(int pollInterval)
{
	m_pollInterval = pollInterval;
}

void ZWaveDeviceManager::setGatewayInfo(SharedPtr<GatewayInfo> info)
{
	m_gatewayInfo = info;
}

void ZWaveDeviceManager::setMessageFactory(
	SharedPtr<ZWaveMessageFactory> factory)
{
	m_factory = factory;
}

void ZWaveDeviceManager::loadDeviceList()
{
	FastMutex::ScopedLock guard(m_lock);
	set<DeviceID> deviceIDs = deviceList(-1);

	for (auto &id : deviceIDs) {
		auto it =
			m_nodesMap.find(nodeIDFromDeviceID(id));

		if (it != m_nodesMap.end())
			it->second.paired = true;
	}

	for (auto it = m_nodesMap.begin(); it != m_nodesMap.end(); ) {
		if (!it->second.paired)
			m_nodesMap.erase(it);
	}
}

uint8_t ZWaveDeviceManager::nodeIDFromDeviceID(
		const DeviceID &deviceID) const
{
	return deviceID.ident() & NODE_ID_MASK;
}
