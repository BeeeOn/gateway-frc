#ifndef BEEEON_UDEV_MONITOR_H
#define BEEEON_UDEV_MONITOR_H

#include <list>
#include <set>
#include <string>

#include <Poco/AtomicCounter.h>
#include <Poco/Timespan.h>

#include "loop/StoppableRunnable.h"
#include "udev/UDevListener.h"
#include "util/Loggable.h"

struct udev;
struct udev_device;
struct udev_monitor;

namespace BeeeOn {

class UDevMonitor : public StoppableRunnable, Loggable {
public:
	UDevMonitor();
	~UDevMonitor();

	void run() override;
	void stop() override;

	void setMatches(const std::list<std::string> &matches);
	void setPollTimeout(const int ms);
	void setIncludeParents(bool enable);
	void registerListener(UDevListener::Ptr listener);

	void initialScan();

private:
	struct udev_monitor *createMonitor();
	struct udev_monitor *doCreateMonitor();
	void collectProperties(
		UDevEvent::Properties &event, struct udev_device *dev) const;
	UDevEvent createEvent(struct udev_device *dev) const;
	void scanDevice(struct udev_monitor *mon);
	void logEvent(const UDevEvent &event, const std::string &action) const;
	void throwFromErrno(const std::string &name);

	void fireAddEvent(const UDevEvent &event);
	void fireRemoveEvent(const UDevEvent &event);
	void fireChangeEvent(const UDevEvent &event);
	void fireMoveEvent(const UDevEvent &event);

private:
	std::list<UDevListener::Ptr> m_listeners;
	std::set<std::string> m_matches;
	Poco::AtomicCounter m_stop;
	Poco::Timespan m_pollTimeout;
	bool m_includeParents;
	struct udev *m_udev;
};

}

#endif