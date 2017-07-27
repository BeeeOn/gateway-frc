#pragma once

#include <map>

#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <Poco/Nullable.h>
#include <Poco/SharedPtr.h>
#include <Poco/Timer.h>

#include <openzwave/Notification.h>

#include "core/DeviceManager.h"
#include "core/GatewayInfo.h"
#include "udev/UDevListener.h"
#include "z-wave/ZWaveDriver.h"

namespace BeeeOn {

class ZWaveMessageFactory;

struct NodeInfo {
	bool paired;
	bool polled;
	std::list<OpenZWave::ValueID> values;
};

/**
 * In OpenZWave, all feedback from the Z-Wave network is sent to the
 * application via callbacks. This class allows the application to add
 * a notification callback handler. All notifications will be reported
 * to it.
 */
class ZWaveDeviceManager : public DeviceManager, public UDevListener {
public:
	ZWaveDeviceManager();
	~ZWaveDeviceManager();

	void run() override;
	void stop() override;

	void onAdd(const UDevEvent &event) override;
	void onRemove(const UDevEvent &event) override;

	bool accept(Command::Ptr cmd) override;
	void handle(Command::Ptr cmd, Answer::Ptr answer) override;

	void setUserPath(const std::string &userPath);
	void setConfigPath(const std::string &configPath);
	void setPollInterval(int pollInterval);
	void setGatewayInfo(Poco::SharedPtr<GatewayInfo> info);
	void setMessageFactory(Poco::SharedPtr<ZWaveMessageFactory> factory);

	void installConfiguration();

	/**
	 * Find data for specific node using node idvoid sendValue(
		const uint8_t nodeId, const std::list<OpenZWave::ValueID> &values);

	uint32_t manufacturerID(uint8_t nodeId);
	uint32_t productID(uint8_t nodeId);
	 * @param nodeId Device identification for Z-Wave network
	 * @return Nullable Object, which contain object with information about node
	 */
	Poco::Nullable<NodeInfo> findNodeInfo(uint8 nodeId);

	Poco::FastMutex& lock();

	/**
	 * It handles notification from Z-Wave network.
	 * @param notification Provides a container for data sent via the notification
	 */
	void onNotification(const OpenZWave::Notification *notification);

private:
	/**
	 * Finding dongle path.
	 */
	std::string dongleMatch(const UDevEvent &e);

	/**
	 * A new node value has been added to OpenZWave's list. These notifications
	 * occur after a node has been discovered.
	 * @param notification Provides a container for data sent via the notification
	 */
	void valueAdded(const OpenZWave::Notification *noticiation);

	/**
	 * A node value has been updated from the Z-Wave and it is different
	 * from the previous value. It creates ZWaveMessage which contains
	 * specific method of product.
	 * @param notification Provides a container for data sent via the notification
	 */
	void valueChanged(const OpenZWave::Notification *noticiation);

	/**
	 * A node value has been removed from OpenZWave's list.
	 * @param notification Provides a container for data sent via the notification
	 */
	void valueRemoved(const OpenZWave::Notification *notification);

	/**
	 * A new node has been added to OpenZWave's list. This may be due to a
	 * device being added to the Z-Wave network, or because the application is
	 * initializing itself.
	 * @param notification Provides a container for data sent via the notification
	 */
	void nodeAdded(const OpenZWave::Notification *notification);

	/**
	 * A node has been removed from OpenZWave's list. This may be due to a device
	 * being removed from the Z-Wave network, or because the application is closing.
	 * @param notification Provides a container for data sent via the notification
	 */
	void nodeRemoved(const OpenZWave::Notification *notification);

	/**
	 * It build DeviceID from homeID, nodeID and gatewayID.
	 */
	DeviceID buildID(uint8_t nodeID);

	uint8_t nodeIDFromDeviceID(const DeviceID &deviceID) const;

	void loadDeviceList();

	void sendValue(
		const uint8_t nodeId, const std::list<OpenZWave::ValueID> &values);

	uint32_t manufacturerID(uint8_t nodeId) const;
	uint32_t productID(uint8_t nodeId) const;

	void stopListen(Poco::Timer &timer);
	void doListenCommand(const Command::Ptr &cmd, const Answer::Ptr &answer);

	void stopUnpair(Poco::Timer &timer);
	void doUnpairCommand(const Command::Ptr &cmd, const Answer::Ptr &answer);

	void doSetValueCommand(const Command::Ptr &cmd, const Answer::Ptr &answer);

private:
	enum State {
		IDLE = 0,
		USB_READY = 1,
		NODE_QUERIED = 2,
	};

private:
	Poco::SharedPtr<GatewayInfo> m_gatewayInfo;
	Poco::FastMutex m_lock;
	ZWaveDriver m_driver;
	std::map<uint8_t, NodeInfo> m_nodesMap;
	uint32_t m_homeID;
	std::string m_dongleName;
	std::string m_userPath;
	std::string m_configPath;
	int m_pollInterval;
	State m_state;
	Poco::SharedPtr<ZWaveMessageFactory> m_factory;

	Poco::TimerCallback<ZWaveDeviceManager> m_listenCallback;
	Poco::AtomicCounter m_isListen;
	Poco::Timer m_listenTimer;

	Poco::TimerCallback<ZWaveDeviceManager> m_callbackUnpair;
	Poco::Timer m_derefUnpair;
};

}
