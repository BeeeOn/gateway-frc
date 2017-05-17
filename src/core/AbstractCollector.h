#pragma once

#include <Poco/Event.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>

#include "core/DistributorListener.h"
#include "loop/StoppableRunnable.h"
#include "util/Loggable.h"
#include "util/StatisticsData.h"
#include "util/StatisticsPrinter.h"

#define DEFAULT_EXPORT_SECONDS 86400 //24 hours

namespace BeeeOn {

/*
 * Abstract class for collecting data for
 * statistics and analysis purpose.
 * */
class AbstractCollector : public DistributorListener, public StoppableRunnable, public Loggable {
public:
	AbstractCollector();
	~AbstractCollector();

	void run() override;
	void stop() override;

	/*
	 * Sets how often should be data exported in seconds.
	 * */
	void setExportInterval(int seconds);
	void setPrinter(Poco::SharedPtr<StatisticsPrinter> printer);

protected:
	virtual std::vector<StatisticsData> computeLastStats(const Poco::Timespan span) = 0;
	virtual int devicesCount() = 0;
	virtual int modulesCount() = 0;

	Poco::Event m_stopEvent;
	Poco::Timespan m_exportInterval;

private:
	/*
	 * Export collected data by implementation specific way.
	 * @param span - This value delimitates time interval
	 * between present and (present minus this value).
	 * Only data within this interval will be exported.
	 * */
	void exportStatistics(const Poco::Timespan span);
	Poco::SharedPtr<StatisticsPrinter> m_printer;
};

}
